terraform {
  required_providers {
    azurerm = {
      source  = "hashicorp/azurerm"
      version = "=2.80.0"
    }
  }
}

# Configure the Microsoft Azure Provider
provider "azurerm" {
  features {}
}
resource "azurerm_resource_group" "tickerboxx" {
  name     = "tickerboxx"
  location = "West Europe"
}

resource "azurerm_container_registry" "acr" {
  name                = "tickerboxx"
  resource_group_name = azurerm_resource_group.tickerboxx.name
  location            = azurerm_resource_group.tickerboxx.location
  sku                 = "basic"
  admin_enabled       = false
}

resource "azurerm_role_assignment" "example" {
  scope                = azurerm_container_registry.acr.id
  role_definition_name = "ACRPull"
  principal_id         = azurerm_app_service.tickerboxx.identity[0].principal_id
}

resource "azurerm_app_service_plan" "tickerboxx" {
  name                = "tickerboxx"
  location            = azurerm_resource_group.tickerboxx.location
  resource_group_name = azurerm_resource_group.tickerboxx.name

  sku {
    tier = "Standard"
    size = "S1"
  }
}

resource "azurerm_app_service" "tickerboxx" {
  name                = "tickerboxx"
  location            = azurerm_resource_group.tickerboxx.location
  resource_group_name = azurerm_resource_group.tickerboxx.name
  app_service_plan_id = azurerm_app_service_plan.tickerboxx.id

  site_config {
    acr_use_managed_identity_credentials  = true
  }

  identity {
    type = "SystemAssigned"
  }

  lifecycle {
    ignore_changes = [
      site_config.0.linux_fx_version # deployments are made outside of Terraform
    ]
  }
}