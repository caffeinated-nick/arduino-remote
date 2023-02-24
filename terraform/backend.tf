terraform {
  backend "s3" {
    bucket = "yourenicked-terraform"
    key    = "remote/tf.state"
    region = "ap-southeast-2"
  }
}
