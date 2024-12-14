#pragma once
#include "CommonInclude.h"
#include "wiGraphicsDevice.h"
#include "wiFFTGenerator.h"
#include "wiScene_Decl.h"
#include "wiMath.h"

namespace wi
{
	class Ocean
	{
	public:
		struct OceanParameters
		{
			// Must be power of 2.
			int dmap_dim = 512;
			// Typical value is 1000 ~ 2000
			float patch_length = 50.0f;

			// Adjust the time interval for simulation.
			float time_scale = 0.3f;
			// Amplitude for transverse wave. Around 1.0
			float wave_amplitude = 1000.0f;
			// Wind direction. Normalization not required.
			XMFLOAT2 wind_dir = XMFLOAT2(0.8f, 0.6f);
			// Around 100 ~ 1000
			float wind_speed = 600.0f;
			// This value damps out the waves against the wind direction.
			// Smaller value means higher wind dependency.
			float wind_dependency = 0.07f;
			// The amplitude for longitudinal wave. Must be positive.
			float choppy_scale = 1.3f;


			XMFLOAT4 waterColor = XMFLOAT4(0.0f, 2.0f / 255.0f, 6.0f / 255.0f, 0.6f);
			XMFLOAT4 extinctionColor = XMFLOAT4(0, 0.9f, 1, 1);
			float waterHeight = 0.0f;
			uint32_t surfaceDetail = 4;
			float surfaceDisplacementTolerance = 2;
		};
		void Create(const OceanParameters& params);

		void UpdateDisplacementMap(wi::graphics::CommandList cmd) const;
		void RenderForOcclusionTest(const wi::scene::CameraComponent& camera, wi::graphics::CommandList cmd) const;
		void Render(const wi::scene::CameraComponent& camera, wi::graphics::CommandList cmd) const;

		void CopyDisplacementMapReadback(wi::graphics::CommandList cmd) const;

		const wi::graphics::Texture* getDisplacementMap() const;
		const wi::graphics::Texture* getGradientMap() const;

		static void Initialize();

		bool IsValid() const { return displacementMap.IsValid(); }

		// occlusion result history bitfield (32 bit->32 frame history)
		mutable uint32_t occlusionHistory = ~0u;
		mutable int occlusionQueries[wi::graphics::GraphicsDevice::GetBufferCount()];
		inline bool IsOccluded() const
		{
			// Perform a conservative occlusion test:
			// If it is visible in any frames in the history, it is determined visible in this frame
			// But if all queries failed in the history, it is occluded.
			// If it pops up for a frame after occluded, it is visible again for some frames
			return occlusionHistory == 0;
		}

		// Return the position at world space modified by the ocean displacement map
		XMFLOAT3 GetDisplacedPosition(const XMFLOAT3& worldPosition) const;

		OceanParameters params;

	protected:
		wi::graphics::Texture displacementMap;		// (RGBA32F)
		wi::graphics::Texture gradientMap;			// (RGBA16F)

		wi::graphics::Texture displacementMap_readback[wi::graphics::GraphicsDevice::GetBufferCount()];		// (RGBA32F)
		mutable bool displacement_readback_valid[arraysize(displacementMap_readback)] = {};
		mutable uint32_t displacement_readback_index = 0;

		void initHeightMap(XMFLOAT2* out_h0, float* out_omega);


		// Initial height field H(0) generated by Phillips spectrum & Gauss distribution.
		wi::graphics::GPUBuffer buffer_Float2_H0;

		// Angular frequency
		wi::graphics::GPUBuffer buffer_Float_Omega;

		// Height field H(t), choppy field Dx(t) and Dy(t) in frequency domain, updated each frame.
		wi::graphics::GPUBuffer buffer_Float2_Ht;

		// Height & choppy buffer in the space domain, corresponding to H(t), Dx(t) and Dy(t)
		wi::graphics::GPUBuffer buffer_Float_Dxyz;


		wi::graphics::GPUBuffer constantBuffer;
		mutable wi::graphics::GPUBuffer indexBuffer;
		mutable wi::graphics::GPUBuffer indexBuffer_occlusionTest;
	};
}