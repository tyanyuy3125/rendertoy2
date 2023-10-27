#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <type_traits>

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
        BBox() = delete;
        BBox(glm::vec3 pmin, glm::vec3 pmax) : _pmin(pmin), _pmax(pmax) {}
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
                node_tree.push_back(BVHNode{(*begin)->GetBoundingBox(), -1, -1});
                return node_tree.size() - 1;
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
        const int RecursiveIntersect(const int node, const glm::vec3 &origin, const glm::vec3 &direction) const
        {
            if (node == -1 || node_tree[node]._bbox.Intersect(origin, direction) == false)
            {
                return -1;
            }
            else if (node < objects.size())
            {
                return node;
            }
            return std::max(RecursiveIntersect(node_tree[node].left, origin, direction), RecursiveIntersect(node_tree[node].right, origin, direction));
        }

    public:
        std::vector<std::unique_ptr<AccelerableObject>> objects;
        void Construct()
        {
            node_tree.resize(objects.size());
            RecursiveConstruct(objects.begin(), objects.end());
        }
        const int Intersect(const glm::vec3 &origin, const glm::vec3 &direction) const
        {
            return RecursiveIntersect(node_tree.size() - 1, origin, direction);
        }
    };
}