terraform {
  backend "s3" {
    bucket = "yourenicked-terraform"
    key    = "fans/tf.state"
    region = "ap-southeast-2"
  }
}