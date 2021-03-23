 # TODO - set up project and all gcloud api permissions with 

resource "google_project_service" "container" {
  project = var.project
  service = "container.googleapis.com"

  disable_dependent_services = true
}

resource "google_project_service" "compute" {
  project = var.project
  service = "compute.googleapis.com"

  disable_dependent_services = true
}

resource "google_project_service" "containerregistry" {
  project = var.project
  service = "containerregistry.googleapis.com"

  disable_dependent_services = true
}

# service account - codefresh needs JSON key to grant permissions to pull from registry
# gcloud iam service-accounts create codefresh-gcr
# gcloud projects add-iam-policy-binding colourapi --member "serviceAccount:codefresh-gcr@colourapi.iam.gserviceaccount.com" --role "roles/storage.Admin"
# gcloud iam service-accounts keys create keyfile.json --iam-account codefresh-gcr@colourapi.iam.gserviceaccount.com

