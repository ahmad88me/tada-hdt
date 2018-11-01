# TADA-HDT
[![Build Status](https://semaphoreci.com/api/v1/ahmad88me/tada-hdt/branches/master/badge.svg)](https://semaphoreci.com/ahmad88me/tada-hdt)
[![codecov](https://codecov.io/gh/ahmad88me/TADA-HDT/branch/master/graph/badge.svg)](https://codecov.io/gh/ahmad88me/TADA-HDT)

This application is intended for processing intermediate files from TADA-NumCol


# GOALS
1. Filter numerical properties :heavy_check_mark:
    - Take as an input a file with class/properties and produce class/numeric-properties
2. Computer the features for each class/numeric-property :fork_and_knife:
    - It will produce a file in the form of class property mean median std

# Output Files Formats
* `class_property_num.tsv`
    CLASS_URI1 | PROP1 | PROP2 | ...
    CLASS_URI2 | PROP3 | PROP4 | ...
* `features.tsv`

    | CLASS_URI | PROPERTY_URI | #NUMERIC_VALS | MEAN | MEDIAN | STD |
    | :-------: | :----------: | :-----------: | :--: | :----: | :-: | 
    

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
