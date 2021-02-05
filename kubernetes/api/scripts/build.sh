#!/bin/bash
SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
me=`basename "$0"`

# tag image with local image tag for use by kind
# see: https://kind.sigs.k8s.io/docs/user/local-registry/
REGISTRY="localhost:5000"

docker build $SCRIPTPATH/.. -t $REGISTRY/api:latest
docker push $REGISTRY/api:latest

echo "$me DONE"