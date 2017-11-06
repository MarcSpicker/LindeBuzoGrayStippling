# Weighted Linde-Buzo-Gray Stippling

An interactive demo application for the algorithm proposed in our SIGGRAPH Asia 2017 technical paper.
Our project website (paper, video, examples) can be found [here](http://graphics.uni-konstanz.de/publikationen/Deussen2017LindeBuzoGray/index.html).

### Disclaimer
This is a reimplemented demo with focus on interactivity, and not the code that was used to generate the images and timings in the paper.


### Abstract
We propose an adaptive version of Lloyd's optimization method that distributes points based on Voronoi diagrams. Our inspiration is the Linde-Buzo-Gray-Algorithm in vector quantization, which dynamically splits Voronoi cells until a desired number of representative vectors is reached. We reformulate this algorithm by splitting and merging Voronoi cells based on their size, greyscale level, or variance of an underlying input image. The proposed method automatically adapts to various constraints and, in contrast to previous work, requires no good initial point distribution or prior knowledge about the final number of points. Compared to weighted Voronoi stippling the convergence rate is much higher and the spectral and spatial properties are superior. Further, because points are created based on local operations, coherent stipple animations can be produced. Our method is also able to produce good quality point sets in other fields, such as remeshing of geometry, based on local geometric features such as curvature.

### Citation
```
@article{Deussen2017LindeBuzoGray,
  address    = {New York, NY, USA},
  articleno  = {233},
  author     = {O. Deussen and M. Spicker and Q. Zheng},
  doi        = {10.1145/3130800.3130819},
  issue_date = {November 2017},
  journal    = {ACM Trans. Graph.},
  keywords   = {lloyd optimization, voronoi diagram, linde-buzo-gray algorithm, stippling, sampling, remeshing},
  month      = {nov},
  number     = {6},
  numpages   = {13},
  pages      = {(to appear)},
  publisher  = {ACM},
  title      = {Weighted Linde-Buzo-Gray Stippling},
  url        = {http://doi.acm.org/10.1145/3130800.3130819},
  volume     = {36},
  year       = {2017},
}
```

### Dependencies
The following libraries are required:
* Qt5Core
* Qt5Widgets
* Qt5Svg

### Building
```bash
mkdir build
cd build
cmake ..
make
./LBGStippling
```