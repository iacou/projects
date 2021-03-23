#!/bin/bash
# script to run codefresh pipeline locally for testing
# see https://codefresh-io.github.io/cli/pipelines/run-pipeline/
# requires codefresh CLI

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
me=`basename "$0"`

codefresh run colourapi/colourapi \
    --local \
    -b codefresh \
    --variable-file $SCRIPTPATH/localvars.yaml \
    --yaml=$SCRIPTPATH/codefresh.yaml

echo "$me DONE"