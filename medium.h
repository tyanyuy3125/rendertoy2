#pragma once

#include <optional>
#include <memory>

#include "rendertoy_internal.h"

namespace rendertoy
{
    // struct PiecewiseMajorantSegement
    // {
    //     const float _tmin, _tmax;
    //     const glm::vec3 _sigma_maj;
    //     PiecewiseMajorantSegement(const float tmin, const float tmax, const glm::vec3 sigma_maj)
    //         : _tmin(tmin), _tmax(tmax), _sigma_maj(sigma_maj) {}
    // };

    // class PiecewiseMajorantIterator
    // {
    // public:
    //     virtual const std::optional<PiecewiseMajorantSegement> Next() = 0;
    // };

    // struct DifferentialVolumeProperties
    // {
    //     const glm::vec3 _sigma_a, _sigma_s;
    //     const std::shared_ptr<PhaseFunction> _phase_func;
    //     const glm::vec3 _Le;
    // };

    // class HomogeneousMajorantIterator : public PiecewiseMajorantIterator
    // {
    // public:
    //     virtual const std::optional<PiecewiseMajorantSegement> Next()
    //     {
    //         if (_called)
    //         {
    //             return {};
    //         }
    //         _called = true;
    //         return _seg;
    //     }

    //     HomogeneousMajorantIterator(const float tmin, const float tmax, const glm::vec3 &sigma_maj)
    //         : _seg(tmin, tmax, sigma_maj), _called(false) {}

    // private:
    //     bool _called = false;
    //     PiecewiseMajorantSegement _seg;
    // };

    class Medium
    {
    public:
        // virtual const DifferentialVolumeProperties SamplePoint(const glm::vec3 &p) const = 0;
        // virtual const std::shared_ptr<PiecewiseMajorantIterator> SampleRay(const glm::vec3 &origin, const glm::vec3 &direction, const float tmax) const = 0;
        virtual const glm::vec3 Tr(const float t) const = 0;
        virtual const glm::vec3 Sample(const glm::vec3 &o, const glm::vec3 &d, const float tmax, VolumeInteraction &v_i) const = 0;
    };

    class HomogeneousMedium : public Medium
    {
    public:
        HomogeneousMedium(const glm::vec3 &sigma_a, const glm::vec3 &sigma_s,
                          const glm::vec3 &Le, const std::shared_ptr<PhaseFunction> &phase_func)
            : _sigma_a(sigma_a), _sigma_s(sigma_s), _phase_func(phase_func) {}
        virtual const glm::vec3 Tr(const float t) const;
        virtual const glm::vec3 Sample(const glm::vec3 &o, const glm::vec3 &d, const float tmax, VolumeInteraction &v_i) const;

    private:
        glm::vec3 _sigma_a, _sigma_s;
        std::shared_ptr<PhaseFunction> _phase_func;
    };
}
