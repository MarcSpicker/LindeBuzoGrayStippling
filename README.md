# Weighted Linde-Buzo-Gray Stippling

An interactive demo application for the algorithm proposed in our SIGGRAPH Asia 2017 technical paper.
Our project website (paper, video, examples) can be found [here](http://graphics.uni-konstanz.de/publikationen/Deussen2017LindeBuzoGray/index.html).

<img src="https://github.com/MarcSpicker/LindeBuzoGrayStippling/blob/master/Bridge.gif?raw=true" alt="Example animation of our algorithm." border="2" align="middle" width="500px" height="375px">

### Disclaimer
This is a reimplemented demo with focus on interactivity, and not the code that was used to generate the images and timings in the paper.


### Abstract
We propose an adaptive version of Lloyd's optimization method that distributes points based on Voronoi diagrams. Our inspiration is the Linde-Buzo-Gray-Algorithm in vector quantization, which dynamically splits Voronoi cells until a desired number of representative vectors is reached. We reformulate this algorithm by splitting and merging Voronoi cells based on their size, greyscale level, or variance of an underlying input image. The proposed method automatically adapts to various constraints and, in contrast to previous work, requires no good initial point distribution or prior knowledge about the final number of points. Compared to weighted Voronoi stippling the convergence rate is much higher and the spectral and spatial properties are superior. Further, because points are created based on local operations, coherent stipple animations can be produced. Our method is also able to produce good quality point sets in other fields, such as remeshing of geometry, based on local geometric features such as curvature.

### Citation
```
@article{Deussen:2017:WLS:3130800.3130819,
  author = {Deussen, Oliver and Spicker, Marc and Zheng, Qian},
  title = {Weighted Linde-Buzo-Gray Stippling},
  journal = {ACM Trans. Graph.},
  issue_date = {November 2017},
  volume = {36},
  number = {6},
  month = nov,
  year = {2017},
  issn = {0730-0301},
  pages = {233:1--233:12},
  articleno = {233},
  numpages = {12},
  url = {http://doi.acm.org/10.1145/3130800.3130819},
  doi = {10.1145/3130800.3130819},
  acmid = {3130819},
  publisher = {ACM},
  address = {New York, NY, USA},
  keywords = {linde-buzo-gray-algorithm, lloyd optimization, remeshing, sampling, stippling, voronoi diagram},
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