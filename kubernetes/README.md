# deploy cluster and api helm chart

Create local cluster

First install Kind, I used this to deploy a local cluster (as k3s not supported on OSX)

https://kind.sigs.k8s.io/docs/user/quick-start/#installation

run script to set up local docker registry for kind and deploy kind cluster:

Run:
```bash
./cluster/kind-with-registry.sh
```

Build the api docker image
```bash
./api/scripts/build.sh
```

When cluster is ready, deploy api helm chart:
```bash
./api/scripts/deploy.sh
```

Helm has been used for this project so as to reduce duplication of code, and to allow for the restriction of ingress by setting ingressEnabled: false for a colour. It also means you can easily add more colours in!

The api web app has been written in python using Flask, primarly because I am comfortable with python and it was quick to get something up and running with Flask.