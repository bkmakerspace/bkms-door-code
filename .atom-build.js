module.exports = {
  cmd: "docker build -t arduino_verify . -f Dockerfile.verify && docker run arduino_verify && docker build -t arduino_uts . -f Dockerfile.unittest && docker run arduino_uts",
  name: "Arduino New",
  sh: true
}
