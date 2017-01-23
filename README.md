# Matrix-vector multiplication on DFE

## Description
A set of DFE algorithms for matrix-vector multiplication written as part of master thesis.

There are three different DFE algorithms - Basic, Transposed and Tiled. All three have also pipelined variants using DFEVector construct ("Vec" at the end of the folder).

For running time comparison there is also simple CPE version for matrix-vector multiplication. 

Example tests are in the bench folder.


## Requirements
DFE algorithms are implemented for and need to be run on [Maxeler platform] (https://www.maxeler.com/solutions/).

Some implementations need to include [Maxeler standard library] (https://github.com/maxeler/maxpower).

## MIT Licence
Repository is open source under the MIT license. See [LICENSE](LICENSE) for details.


