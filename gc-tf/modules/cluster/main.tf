provider "google" {
  project = var.project
}

resource "google_service_account" "default" {
  account_id   = "service-account-id"
  display_name = "k8s Service Account"
}

resource "google_container_cluster" "primary" {
  name     = "iacou"
  location = "us-central1"

  # We can't create a cluster with no node pool defined, but we want to only use
  # separately managed node pools. So we create the smallest possible default
  # node pool and immediately delete it.
  remove_default_node_pool = true
  initial_node_count       = 1
}

resource "google_container_node_pool" "primary_preemptible_nodes" {
  name       = "iacou-primary"
  location   = "us-central1"
  cluster    = google_container_cluster.primary.name
  node_count = 1

  node_config {
    preemptible  = true
    machine_type = "e2-medium"

    # Google recommends custom service accounts that have cloud-platform scope and permissions granted via IAM Roles.
    service_account = google_service_account.default.email
    oauth_scopes = [
      "https://www.googleapis.com/auth/cloud-platform"
    ]
  }
}

resource "google_compute_global_address" "ingress" {
  name = "ingress-ip"
}

# codefresh needs basic auth enabled on cluster which is deprecated in gks 1.19
# grant storage object admin permissions to the cluster service account so that it can pull from GCR