#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <type_traits>
#include <algorithm>
#include <stack>
#include <limits>

#include "rendertoy_internal.h"
#include "intersectinfo.h"

namespace rendertoy
{
    class BBox
    {
    private:
        glm::vec3 _pmin;
        glm::vec3 _pmax;

    public:
        BBox(glm::vec3 pmin, glm::vec3 pmax) : _pmin(pmin), _pmax(pmax) {}
        BBox() : BBox(glm::vec3{0.0f}, glm::vec3{0.0f}) {}
        const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, float &t) const;
        const glm::vec3 GetCenter() const;
        void Union(const BBox &a);
        const int GetLongestAxis() const;
    };

    template <typename T>
    struct has_bounding_box
    {
    private:
        template <typename U>
        static auto test(U *p) -> decltype(p->GetBoundingBox(), std::true_type{});

        template <typename U>
        static std::false_type test(...);

    public:
        static constexpr bool value = decltype(test<T>(nullptr))::value;
    };

    struct BVHNode
    {
        BBox _bbox;
        int left = -1;
        int right = -1;
    };

    template <typename AccelerableObject, std::enable_if_t<has_bounding_box<AccelerableObject>::value, bool> _ = true>
    class BVH
    {
    private:
        std::vector<BVHNode> node_tree;
        const int RecursiveConstruct(std::vector<std::shared_ptr<AccelerableObject>>::iterator begin, std::vector<std::shared_ptr<AccelerableObject>>::iterator end)
        {
            if (begin + 1 == end)
            {
                node_tree[std::distance(objects.begin(), begin)]._bbox = (*begin)->GetBoundingBox();
                return static_cast<int>(std::distance(objects.begin(), begin));
            }

            BBox overall_bbox = (*begin)->GetBoundingBox();
            for (auto it = begin; it != end; ++it)
            {
                overall_bbox.Union((*it)->GetBoundingBox());
            }
            int longest_axis = overall_bbox.GetLongestAxis();
            std::sort(begin, end, [&](const std::shared_ptr<AccelerableObject> &a, const std::shared_ptr<AccelerableObject> &b) -> bool
                      { return a->GetBoundingBox().GetCenter()[longest_axis] < b->GetBoundingBox().GetCenter()[longest_axis]; });

            auto mid = std::next(begin, std::distance(begin, end) / 2);
            int left_node = RecursiveConstruct(begin, mid);
            int right_node = RecursiveConstruct(mid, end);
            node_tree.push_back(BVHNode{overall_bbox, left_node, right_node});

            return static_cast<int>(node_tree.size()) - 1;
        }

    public:
        BVH() = default;
        BVH(const BVH &) = delete;

        std::vector<std::shared_ptr<AccelerableObject>> objects;
        void Construct()
        {
            node_tree.resize(objects.size());
            RecursiveConstruct(objects.begin(), objects.end());
        }
        const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const
        {
            IntersectInfo temp_intersect_info;
            int closest_index = -1;
// #define DISABLE_BVH
#ifdef DISABLE_BVH // For debug purposes.
            for (int i = 0; i < objects.size(); ++i)
            {
                if (objects[i]->Intersect(origin, direction, temp_intersect_info))
                {
                    if (closest_index == -1 || temp_intersect_info._t < intersect_info._t)
                    {
                        intersect_info = temp_intersect_info;
                        closest_index = i;
                    }
                }
            }
#else
            std::stack<int> traverse_stack;
            traverse_stack.push(static_cast<int>(node_tree.size()) - 1);
            while (!traverse_stack.empty())
            {
                auto stack_top = traverse_stack.top();
                traverse_stack.pop();
// #define DISABLE_BVH_DISTANCE_OPTIMIZATION
#ifdef DISABLE_BVH_DISTANCE_OPTIMIZATION
                float deprecated_dist;
                if (node_tree[stack_top]._bbox.Intersect(origin, direction, deprecated_dist))
                {
                    if (stack_top < objects.size())
                    {
                        if (objects[stack_top]->Intersect(origin, direction, temp_intersect_info))
                        {
                            if (closest_index == -1 || temp_intersect_info._t < intersect_info._t)
                            {
                                intersect_info = temp_intersect_info;
                                closest_index = stack_top;
                            }
                        }
                    }
                    else
                    {
                        if (node_tree[stack_top].right != -1)
                        {
                            traverse_stack.push(node_tree[stack_top].right);
                        }
                        if (node_tree[stack_top].left != -1)
                        {
                            traverse_stack.push(node_tree[stack_top].left);
                        }
                    }
                }
#else
                if (stack_top < objects.size())
                {
                    if (objects[stack_top]->Intersect(origin, direction, temp_intersect_info))
                    {
                        if (closest_index == -1 || temp_intersect_info._t < intersect_info._t)
                        {
                            intersect_info = temp_intersect_info;
                            closest_index = stack_top;
                        }
                    }
                }
                else
                {
                    float dist_l = std::numeric_limits<float>::infinity(), dist_r = std::numeric_limits<float>::infinity();
                    bool intersect_l = false, intersect_r = false;
                    if(node_tree[stack_top].left != -1)
                    {
                        intersect_l = node_tree[node_tree[stack_top].left]._bbox.Intersect(origin, direction, dist_l);
                    }
                    if(node_tree[stack_top].right != -1)
                    {
                        intersect_r = node_tree[node_tree[stack_top].right]._bbox.Intersect(origin, direction, dist_r);
                    }
                    if(intersect_l && intersect_r)
                    {
                        if(dist_l < dist_r)
                        {
                            traverse_stack.push(node_tree[stack_top].right);
                            traverse_stack.push(node_tree[stack_top].left); // So left is on the top.
                        }
                        else
                        {
                            traverse_stack.push(node_tree[stack_top].left);
                            traverse_stack.push(node_tree[stack_top].right);
                        }
                    }
                    else if(intersect_l && !intersect_r)
                    {
                        traverse_stack.push(node_tree[stack_top].left);
                    }
                    else if(!intersect_l && intersect_r)
                    {
                        traverse_stack.push(node_tree[stack_top].right);
                    }
                }
#endif // DISABLE_BVH_DISTANCE_OPTIMIZATION
            }
#endif // DISABLE_BVH
            if (closest_index == -1)
            {
                return false;
            }
            return true;
        }
    };
}