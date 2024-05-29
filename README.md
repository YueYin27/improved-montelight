# Improved Monte Carlo Methods for Photorealistic Rendering

## Table of Contents
- [Overview](#overview)
- [Implementation Details](#implementation-details)
- [How to Run](#how-to-run)
- [Experiemental Results](#experiemental-results)
- [Ablation Study](#ablation-study)

## Overview
This project is based on the paper [Monte Carlo methods for improved rendering](chrome-extension://efaidnbmnnnibpcajpcglclefindmkaj/https://smerity.com/montelight-cpp/files/AM207_Paper.pdf). Our work enhances the Monte Carlo rendering methods to improve the efficiency and quality of photorealistic image rendering. The primary advancements include adaptive sampling, improved Russian Roulette termination, and extended support for various materials and geometries. These methods are integrated into a ray tracing framework and evaluated on scenes with varying complexity.

## Implementation Details
### Enhanced Algorithms Implemented
1. **Adaptive Sampling**
   - Dynamically adjusts the number of samples based on the observed variance to focus computational resources on more complex areas.
2. **Improved Russian Roulette Process**
   - Refines the termination process by including a scaling operation to maintain the integral properties of radiance.
3. **Extended Material Support**
   - Includes materials like diffuse, mirror, and glass to handle different light interactions.
4. **Additional Geometry Support**
   - Supports geometries like spheres, cubes, planes, and patterned surfaces.

### Key Libraries
- C++
- Standard Template Library (STL)
- Custom Vector, Ray, Image, and Shape classes

## How to Run
1. **Clone the repository**:
    ```bash
    git clone https://github.com/YueYin27/improved-montelight.git
    cd improved-montelight
    ```

2. **Compile the code**:
    ```bash
    g++ -o render main.cpp
    ```

3. **Run the renderer**:
    ```bash
    ./render <width> <height> <adaptive_sampling> [<max_spp> <min_spp>]
    ```
    - `<width>`: Width of the output image.
    - `<height>`: Height of the output image.
    - `<adaptive_sampling>`: Enable adaptive sampling (`true` or `false`).
    - `<max_spp>` (optional): Maximum samples per pixel.
    - `<min_spp>` (optional): Minimum samples per pixel.

## Experiemental Results
The enhanced Monte Carlo rendering methods demonstrate significant improvements in both efficiency and image quality. Below are some sample rendering results:

### Comparative Results
![Comparative results of different path tracing techniques.](readme_figures/comparison.png)
*Figure 1: Comparative results of different path tracing techniques. The first row features a simple scene with only diffuse materials, while the second row showcases a complex scene with additional geometries and materials like mirrors and glass. (a) and (b) display results with maximum path depths of 1 and 2, appearing dark. (c) uses the original Russian roulette path termination method same as [Monte Carlo methods for improved rendering](chrome-extension://efaidnbmnnnibpcajpcglclefindmkaj/https://smerity.com/montelight-cpp/files/AM207_Paper.pdf), resulting in better illumination. (d) employs an improved method with scaling to maintain energy balance, producing the brightest and most pleasant results.*

### Variance Threshold Impact
![Impact of different variance thresholds in adaptive sampling.](readme_figures/adaptive.png)
*Figure 2: Comparative results of different variance threshold τ values. The zoom-in regions highlight the impact of varying the variance threshold τ in adaptive sampling, showing that smaller τ values result in finer details and reduced granularity.*

## Ablation Study
The ablation study evaluates the impact of adaptive sampling and scaling after Russian Roulette on rendering quality. The results show that both techniques are crucial for achieving the best illumination and detail in rendered images.

![Ablation study results.](readme_figures/ablation.png)
*Figure 4: Ablation study results. (a) and (e) show results without adaptive sampling and without scaling; (b) and (f) show results with adaptive sampling but without scaling; (c) and (g) show results without adaptive sampling but with scaling; and (d) and (h) show results with both adaptive sampling and scaling. Both adaptive sampling and scaling are important, with scaling having a more pronounced effect on overall illumination and quality.*

## Conclusion
The project introduces several enhancements to Monte Carlo rendering methods, focusing on adaptive sampling, improved Russian Roulette processes, and support for complex materials and geometries. These advancements significantly improve the efficiency and quality of rendered images, making Monte Carlo methods more practical for high-resolution and complex scenes.
