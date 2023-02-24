/* resource "aws_acm_certificate" "yourenicked_cert" {
  private_key        = file("./tls/cert-key.pem")
  certificate_body   = file("./tls/cert.pem")
  certificate_chain  = file("./tls/cert-chain.pem")

  /* tags = {
    project = var.tags_project
  } */
/* } */

/* data "aws_acm_certificate" "yourenicked_cert" {
  domain   = "*.yourenicked.net"
  statuses = ["ISSUED"]
} */
