workflow "Build and verify on push" {
  on = "push"
  resolves = ["GitHub Action for Docker-1"]
}

action "GitHub Action for Docker" {
  uses = "actions/docker/cli@8cdf801b322af5f369e00d85e9cf3a7122f49108"
  args = "build -t pipakin/bkms_door_code_verify . -f Dockerfile.verify"
}

action "GitHub Action for Docker-1" {
  uses = "actions/docker/cli@8cdf801b322af5f369e00d85e9cf3a7122f49108"
  needs = ["GitHub Action for Docker"]
  args = "run pipakin/bkms_door_code_verify"
}
