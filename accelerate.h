#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <type_traits>
#include <algorithm>
#include <stack>

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
        const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction) const;
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
        const int RecursiveConstruct(std::vector<std::unique_ptr<AccelerableObject>>::iterator begin, std::vector<std::unique_ptr<AccelerableObject>>::iterator end)
        {
            if (begin + 1 == end)
            {
                return std::distance(objects.begin(), begin);
            }

            BBox overall_bbox = (*begin)->GetBoundingBox();
            for (auto it = begin; it != end; ++it)
            {
                overall_bbox.Union((*it)->GetBoundingBox());
            }
            int longest_axis = overall_bbox.GetLongestAxis();
            std::sort(objects.begin(), objects.end(), [&](const std::unique_ptr<AccelerableObject> &a, const std::unique_ptr<AccelerableObject> &b) -> bool
                      { return a->GetBoundingBox().GetCenter()[longest_axis] < b->GetBoundingBox().GetCenter()[longest_axis]; });

            auto mid = std::next(begin, std::distance(begin, end) / 2);
            int left_node = RecursiveConstruct(begin, mid);
            int right_node = RecursiveConstruct(mid, end);
            node_tree.push_back(BVHNode{overall_bbox, left_node, right_node});

            return node_tree.size() - 1;
        }

    public:
        BVH() = default;
        BVH(const BVH&) = delete;

        std::vector<std::unique_ptr<AccelerableObject>> objects;
        void Construct()
        {
            node_tree.resize(objects.size());
            for(int i = 0;i<objects.size();++i)
            {
                node_tree[i]._bbox = objects[i]->GetBoundingBox();
            }
            RecursiveConstruct(objects.begin(), objects.end());
        }
        const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const
        {
            IntersectInfo temp_intersect_info;
            int closest_index = -1;

            std::stack<int> traverse_stack;
            traverse_stack.push(node_tree.size() - 1);
            while(!traverse_stack.empty())
            {
                auto stack_top = traverse_stack.top();
                traverse_stack.pop();
                if(node_tree[stack_top]._bbox.Intersect(origin, direction))
                {
                    if(stack_top < objects.size())
                    {
                        if(objects[stack_top]->Intersect(origin, direction, temp_intersect_info))
                        {
                            if(closest_index == -1 || temp_intersect_info._t < intersect_info._t)
                            {
                                intersect_info = temp_intersect_info;
                                closest_index = stack_top;
                            }
                        }
                    }
                    else
                    {
                        if(node_tree[stack_top].right!=-1)
                        {
                            traverse_stack.push(node_tree[stack_top].right);
                        }
                        if(node_tree[stack_top].left!=-1)
                        {
                            traverse_stack.push(node_tree[stack_top].left);
                        }
                    }
                }
            }

            if(closest_index == -1)
            {
                return false;
            }
            return true;
        }
    };
}