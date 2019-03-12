workflow "Build and verify on push" {
  on = "push"
  resolves = ["Unit Tests"]
}

action "Verification Docker Build" {
  uses = "actions/docker/cli@8cdf801b322af5f369e00d85e9cf3a7122f49108"
  args = "build -t pipakin/bkms_door_code_verify . -f Dockerfile.verify"
}

action "Arduino Verify" {
  uses = "actions/docker/cli@8cdf801b322af5f369e00d85e9cf3a7122f49108"
  args = "run pipakin/bkms_door_code_verify"
  needs = ["Verification Docker Build"]
}

action "Unit Test Docker Build" {
  uses = "actions/docker/cli@8cdf801b322af5f369e00d85e9cf3a7122f49108"
  needs = ["Arduino Verify"]
  args = " build -t pipakin/bkms_door_code_uts . -f Dockerfile.unittest"
}

action "Unit Tests" {
  uses = "actions/docker/cli@8cdf801b322af5f369e00d85e9cf3a7122f49108"
  needs = ["Unit Test Docker Build"]
  args = "docker run pipakin/bkms_door_code_uts"
}
