#!/bin/bash
SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
me=`basename "$0"`

helm upgrade --install \
    api-chart $SCRIPTPATH/../helm/charts

echo "$me DONE"
