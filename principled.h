#pragma once

// materials/disney.h*
#include "material.h"
#include "rendertoy_internal.h"

namespace rendertoy
{
    class PrincipledBSDF : public IMaterial
    {
    public:
        // PrincipledBSDF Public Methods
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
                       const std::shared_ptr<ISamplableColor> &scatterDistance,
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
              scatterDistance(scatterDistance),
              thin(thin),
              flatness(flatness),
              diffTrans(diffTrans) {}

    private:
        // PrincipledBSDF Private Data
        std::shared_ptr<ISamplableColor> color;
        std::shared_ptr<ISamplableNumerical> metallic, eta;
        std::shared_ptr<ISamplableNumerical> roughness, specularTint, anisotropic, sheen;
        std::shared_ptr<ISamplableNumerical> sheenTint, clearcoat, clearcoatGloss;
        std::shared_ptr<ISamplableNumerical> specTrans;
        std::shared_ptr<ISamplableColor> scatterDistance;
        bool thin;
        std::shared_ptr<ISamplableNumerical> flatness, diffTrans, bumpMap;
    };

}
