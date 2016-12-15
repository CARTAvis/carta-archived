Carta Viewer
=======

Visualization Code for ALMA and SKA

#### Wiki:
https://github.com/CARTAvis/carta/wiki

#### Branching model
`master`:  
mainstream branch, no development.

`develop`:  
development branch, usually will merge feature branch to it and include hot fix for those feature. After each phase release, merge `develop` to `master`.  

`each feature branch`:  
people develop each feature in its branch whose name can be `peter/drawImage` or `issue-131` if we use tickets. When it is finished, use `pull request` to proceed code review and then merge to develop. After merging, evaluate those added features on `develop`. 

`Fix Bug`:
1. Except some special cases, such as modifying documents, changing build scripts, low/no-risk fixes, otherwise you need to commit your bug fixes in Hotfix branch or the original feature branch, then make a pull request to do code review.
