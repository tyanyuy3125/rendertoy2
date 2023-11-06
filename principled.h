#pragma once

#include "material.h"
#include "rendertoy_internal.h"

namespace rendertoy
{
    class PrincipledBSDF : public IMaterial
    {
    public:
        PrincipledBSDF(const std::shared_ptr<ISamplableColor> &albedo,
                       const std::shared_ptr<ISamplableNumerical> &metallic,
                       const std::shared_ptr<ISamplableNumerical> &eta,
                       const std::shared_ptr<ISamplableNumerical> &roughness,
                       const std::shared_ptr<ISamplableNumerical> &specularTint,
                       const std::shared_ptr<ISamplableNumerical> &anisotropic,
                       const std::shared_ptr<ISamplableNumerical> &sheen,
                       const std::shared_ptr<ISamplableNumerical> &sheenTint,
                       const std::shared_ptr<ISamplableNumerical> &clearcoat,
                       const std::shared_ptr<ISamplableNumerical> &clearcoatGloss,
                       const std::shared_ptr<ISamplableNumerical> &specTrans,
                       bool thin,
                       const std::shared_ptr<ISamplableNumerical> &flatness,
                       const std::shared_ptr<ISamplableNumerical> &diffTrans)
            : IMaterial(albedo),
              metallic(metallic),
              eta(eta),
              roughness(roughness),
              specularTint(specularTint),
              anisotropic(anisotropic),
              sheen(sheen),
              sheenTint(sheenTint),
              clearcoat(clearcoat),
              clearcoatGloss(clearcoatGloss),
              specTrans(specTrans),
              thin(thin),
              flatness(flatness),
              diffTrans(diffTrans) {}

        virtual const std::unique_ptr<BSDF> GetBSDF(const IntersectInfo &intersect_info) const;

    private:
        std::shared_ptr<ISamplableNumerical> metallic, eta;
        std::shared_ptr<ISamplableNumerical> roughness, specularTint, anisotropic, sheen;
        std::shared_ptr<ISamplableNumerical> sheenTint, clearcoat, clearcoatGloss;
        std::shared_ptr<ISamplableNumerical> specTrans;
        bool thin;
        std::shared_ptr<ISamplableNumerical> flatness, diffTrans, bumpMap;
    };

}
