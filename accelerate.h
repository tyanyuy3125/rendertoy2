#pragma once

#include <vector>
#include <memory>
#include <type_traits>
#include <algorithm>
#include <stack>
#include <limits>

#include "rendertoy_internal.h"
#include "intersectinfo.h"
#include "logger.h"

#ifdef USE_EXT_BVH
#include <bvh/v2/bvh.h>
#include <bvh/v2/vec.h>
#include <bvh/v2/ray.h>
#include <bvh/v2/node.h>
#include <bvh/v2/default_builder.h>
#include <bvh/v2/thread_pool.h>
#include <bvh/v2/executor.h>
#include <bvh/v2/stack.h>
#include <bvh/v2/tri.h>
#endif // USE_EXT_BVH

namespace rendertoy
{
    class BBox
    {
    public:
        glm::vec3 _pmin;
        glm::vec3 _pmax;

    public:
        BBox(glm::vec3 pmin, glm::vec3 pmax) : _pmin(pmin), _pmax(pmax) {}
        BBox() : BBox(glm::vec3{std::numeric_limits<float>::max()}, glm::vec3{std::numeric_limits<float>::min()}) {}
        const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, float &t) const;
        const glm::vec3 GetCenter() const;
        void Union(const BBox &a);
        void Union(const glm::vec3 &p);
        const int GetLongestAxis() const;
        const glm::vec3 Diagonal() const;
        const float SurfaceArea() const;
        const glm::vec3 Offset(const glm::vec3 &p) const;
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

#define USE_SAH

    struct BVHNode
    {
        BBox _bbox;
        int left = -1;
        int right = -1;
#ifdef USE_SAH
        int first_primitive_offset = -1;
        int last_primitive_offset = -1;
#endif // USE_SAH
    };

#ifdef USE_SAH
    struct BVHSplitBucket
    {
        int count = 0;
        BBox bounds;
    };
#endif // USE_SAH

#ifdef USE_EXT_BVH

    template <typename AccelerableObject, std::enable_if_t<has_bounding_box<AccelerableObject>::value, bool> _ = true>
    class BVH
    {
        using Scalar = float;
        using BVH_BBox = bvh::v2::BBox<Scalar, 3>;
        using Node = bvh::v2::Node<Scalar, 3>;
        using Vec3 = bvh::v2::Vec<Scalar, 3>;
        using Bvh = bvh::v2::Bvh<Node>;
        using Ray = bvh::v2::Ray<Scalar, 3>;

        static const Vec3 Vec3Convert(const glm::vec3 &vec)
        {
            return Vec3(vec.x, vec.y, vec.z);
        }

        static const BVH_BBox BBoxConvert(const BBox &bbox)
        {
            return BVH_BBox(Vec3Convert(bbox._pmin), Vec3Convert(bbox._pmax));
        }

    private:
    public:
        BVH() = default;
        BVH(const BVH &) = delete;

        std::vector<std::shared_ptr<AccelerableObject>> objects;
        std::vector<std::shared_ptr<AccelerableObject>> precomputed_objects;
        Bvh internal_bvh;
        void Construct()
        {
            bvh::v2::ThreadPool thread_pool;
            bvh::v2::ParallelExecutor executor(thread_pool);

            std::vector<BVH_BBox> bboxes(objects.size());
            std::vector<Vec3> centers(objects.size());
            executor.for_each(0, objects.size(), [&](size_t begin, size_t end)
                              {
                                for (size_t i = begin; i < end; ++i) {
                                    bboxes[i]  = BBoxConvert(objects[i]->GetBoundingBox());
                                    centers[i] = Vec3Convert(objects[i]->GetCenter());
                                } });

            typename bvh::v2::DefaultBuilder<Node>::Config config;
            config.quality = bvh::v2::DefaultBuilder<Node>::Quality::High;
            internal_bvh = bvh::v2::DefaultBuilder<Node>::build(thread_pool, bboxes, centers, config);
        }
        const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const
        {
            auto ray = Ray{
                Vec3Convert(origin),                   // Ray origin
                Vec3Convert(direction),                // Ray direction
                0.0f,                                  // Minimum intersection distance
                std::numeric_limits<float>::infinity() // Maximum intersection distance
            };
            static constexpr size_t stack_size = 64;
            static constexpr bool use_robust_traversal = false;
            bvh::v2::SmallStack<Bvh::Index, stack_size> stack;
            IntersectInfo temp_intersect_info;
            temp_intersect_info._time = intersect_info._time; // 时间要保持一致
            int closest_index = -1;
            internal_bvh.intersect<false, use_robust_traversal>(ray, internal_bvh.get_root().index, stack,
                                                                [&](size_t begin, size_t end)
                                                                {
                                                                    for (size_t i = begin; i < end; ++i)
                                                                    {
                                                                        auto j = internal_bvh.prim_ids[i];
                                                                        auto hit = objects[j]->Intersect(origin, direction, temp_intersect_info);
                                                                        if (hit)
                                                                        {
                                                                            if (closest_index == -1 || temp_intersect_info._t < intersect_info._t)
                                                                            {
                                                                                intersect_info = temp_intersect_info;
                                                                                closest_index = j;
                                                                            }
                                                                        }
                                                                    }
                                                                    return closest_index != -1;
                                                                });
            if (closest_index == -1)
            {
                return false;
            }
            return true;
        }
    };

#else
    template <typename AccelerableObject, std::enable_if_t<has_bounding_box<AccelerableObject>::value, bool> _ = true>
    class BVH
    {
    public:
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
            auto mid = begin;
#ifdef USE_SAH
            BBox centroid_bbox((*begin)->GetBoundingBox().GetCenter(), (*begin)->GetBoundingBox().GetCenter());
            for (auto it = begin; it != end; ++it)
            {
                centroid_bbox.Union((*it)->GetBoundingBox().GetCenter());
            }
            int dim = centroid_bbox.GetLongestAxis();

            if (centroid_bbox._pmax[dim] == centroid_bbox._pmin[dim])
            {
                node_tree.push_back(BVHNode{overall_bbox,
                                            -1,
                                            -1,
                                            static_cast<int>(std::distance(objects.begin(), begin)),
                                            static_cast<int>(std::distance(objects.begin(), end)) - 1});
                return static_cast<int>(node_tree.size()) - 1;
            }

            std::sort(begin, end, [&](const std::shared_ptr<AccelerableObject> &a, const std::shared_ptr<AccelerableObject> &b) -> bool
                      { return a->GetBoundingBox().GetCenter()[dim] < b->GetBoundingBox().GetCenter()[dim]; });
            if (std::distance(begin, end) <= 2)
            {
                // begin + 1 == end case has been dealt before.
                mid = std::next(begin, std::distance(begin, end) / 2);
            }
            else
            {
                constexpr int N_BUCKETS = 24;
                BVHSplitBucket buckets[N_BUCKETS];
                for (auto prim = begin; prim != end; ++prim)
                {
                    int b = static_cast<int>(N_BUCKETS * centroid_bbox.Offset((*prim)->GetBoundingBox().GetCenter())[dim]);
                    if (b == N_BUCKETS)
                    {
                        b = N_BUCKETS - 1;
                    }
                    buckets[b].count++;
                    buckets[b].bounds.Union((*prim)->GetBoundingBox());
                }

                constexpr int N_SPLITS = N_BUCKETS - 1;
                float costs[N_SPLITS] = {};
                std::fill(&costs[0], &costs[N_SPLITS - 1], 0.0f);

                int count_below = 0;
                BBox bound_below = buckets[0].bounds;
                for (int i = 0; i < N_SPLITS; ++i)
                {
                    bound_below.Union(buckets[i].bounds);
                    count_below += buckets[i].count;
                    costs[i] += count_below * bound_below.SurfaceArea();
                }

                int count_above = 0;
                BBox bound_above = buckets[N_SPLITS].bounds;
                for (int i = N_SPLITS; i >= 1; --i)
                {
                    bound_above.Union(buckets[i].bounds);
                    count_above += buckets[i].count;
                    costs[i - 1] += count_above * bound_above.SurfaceArea();
                }

                int min_cost_split_bucket = -1;
                float min_cost = std::numeric_limits<float>::infinity();
                for (int i = 0; i < N_SPLITS; ++i)
                {
                    if (costs[i] < min_cost)
                    {
                        min_cost = costs[i];
                        min_cost_split_bucket = i;
                    }
                }
                float leaf_cost = static_cast<float>(std::distance(begin, end));
                min_cost = 1.0f / 2.0f + min_cost / overall_bbox.SurfaceArea();

                if (std::distance(begin, end) > 16 || min_cost < leaf_cost)
                {
                    mid = std::stable_partition(
                        begin, end,
                        [=](const std::shared_ptr<AccelerableObject> &bp)
                        {
                            int b = static_cast<int>(N_BUCKETS * centroid_bbox.Offset(bp->GetBoundingBox().GetCenter())[dim]);
                            if (b == N_BUCKETS)
                            {
                                b = N_BUCKETS - 1;
                            }
                            return b <= min_cost_split_bucket;
                        });
                }
                else
                {
                    node_tree.push_back(BVHNode{overall_bbox,
                                                -1,
                                                -1,
                                                static_cast<int>(std::distance(objects.begin(), begin)),
                                                static_cast<int>(std::distance(objects.begin(), end)) - 1});
                    return static_cast<int>(node_tree.size()) - 1;
                }
            }
#else
            int longest_axis = overall_bbox.GetLongestAxis();
            std::sort(begin, end, [&](const std::shared_ptr<AccelerableObject> &a, const std::shared_ptr<AccelerableObject> &b) -> bool
                      { return a->GetBoundingBox().GetCenter()[longest_axis] < b->GetBoundingBox().GetCenter()[longest_axis]; });
            mid = std::next(begin, std::distance(begin, end) / 2);
#endif // USE_SAH
            int left_node = RecursiveConstruct(begin, mid);
            int right_node = RecursiveConstruct(mid, end);
#ifdef USE_SAH
            node_tree.push_back(BVHNode{overall_bbox,
                                        left_node,
                                        right_node,
                                        static_cast<int>(std::distance(objects.begin(), begin)),
                                        static_cast<int>(std::distance(begin, end)) - 1});
#else
            node_tree.push_back(BVHNode{overall_bbox,
                                        left_node,
                                        right_node});
#endif             // USE_SAH

            return static_cast<int>(node_tree.size()) - 1;
        }

    public:
        BVH() = default;
        BVH(const BVH &) = delete;

        std::vector<std::shared_ptr<AccelerableObject>> objects;
        void Construct()
        {
            if (objects.empty())
            {
                return;
            }
            node_tree.resize(objects.size());
            RecursiveConstruct(objects.begin(), objects.end());
        }
        const bool Intersect(const glm::vec3 &origin, const glm::vec3 &direction, IntersectInfo RENDERTOY_FUNC_ARGUMENT_OUT intersect_info) const
        {
            if (node_tree.empty())
            {
                return false;
            }
            IntersectInfo temp_intersect_info;
            temp_intersect_info._time = intersect_info._time; // 时间要保持一致
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
#ifdef USE_SAH
                else if (node_tree[stack_top].left == -1 && node_tree[stack_top].right == -1)
                {
                    for (int prim_idx = node_tree[stack_top].first_primitive_offset; prim_idx <= node_tree[stack_top].last_primitive_offset; ++prim_idx)
                    {
                        if (objects[prim_idx]->Intersect(origin, direction, temp_intersect_info))
                        {
                            if (closest_index == -1 || temp_intersect_info._t < intersect_info._t)
                            {
                                intersect_info = temp_intersect_info;
                                closest_index = stack_top;
                            }
                        }
                    }
                }
#endif // USE_SAH
                else
                {
                    float dist_l = std::numeric_limits<float>::infinity(), dist_r = std::numeric_limits<float>::infinity();
                    bool intersect_l = false, intersect_r = false;
                    if (node_tree[stack_top].left != -1)
                    {
                        intersect_l = node_tree[node_tree[stack_top].left]._bbox.Intersect(origin, direction, dist_l);
                    }
                    if (node_tree[stack_top].right != -1)
                    {
                        intersect_r = node_tree[node_tree[stack_top].right]._bbox.Intersect(origin, direction, dist_r);
                    }
                    if (intersect_l && intersect_r)
                    {
                        if (dist_l < dist_r)
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
                    else if (intersect_l && !intersect_r)
                    {
                        traverse_stack.push(node_tree[stack_top].left);
                    }
                    else if (!intersect_l && intersect_r)
                    {
                        traverse_stack.push(node_tree[stack_top].right);
                    }
                }
            }
#endif // DISABLE_BVH
            if (closest_index == -1)
            {
                return false;
            }
            return true;
        }
    };
#endif // USE_EXT_BVH
}