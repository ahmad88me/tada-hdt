# TADA-HDT
[![Build Status](https://semaphoreci.com/api/v1/ahmad88me/tada-hdt/branches/master/badge.svg)](https://semaphoreci.com/ahmad88me/tada-hdt)
[![codecov](https://codecov.io/gh/ahmad88me/TADA-HDT/branch/master/graph/badge.svg)](https://codecov.io/gh/ahmad88me/TADA-HDT)
[![cirrus](https://api.cirrus-ci.com/github/ahmad88me/tada-hdt.svg)](https://api.cirrus-ci.com/github/ahmad88me/tada-hdt)
[![status](https://img.shields.io/badge/status-under%20development-ff69b4.svg)](https://github.com/ahmad88me/tada-hdt)


This application is intended for processing intermediate files from TADA-NumCol


# Output Files Formats
* `class_property_num.tsv`

    | CLASS_URI1 | PROP1 | PROP2 | ... |
    | :--------: | :---: | :---: | :-: |
    | CLASS_URI2 | PROP3 | PROP4 | ... |

* `features.tsv`

    | CLASS_URI | PROPERTY_URI | #NUMERIC_VALS | MEAN | MEDIAN | STD |
    | :-------: | :----------: | :-----------: | :--: | :----: | :-: | 

# Workflow
![alt text](workflow.svg)   

# Install
(on ubuntu only) See `scripts/setup.sh` 

# Build
`make`


# Run
`make run`

# Debug
`make debug`
It will generate massif output

# Tests
`make test`

# Coverage
`test cov`

# Docker, automated tests and coverage
(docker should be installed)
`sh scripts/run_test_with_docker.sh`
