/* resource "aws_route53_record" "www-fans" {
  zone_id = "Z3JE0YNN12YBF9"
  name    = var.fans-site-bucket-name
  type    = "A"
  alias {
    name                   = aws_cloudfront_distribution.fans_yourenicked_s3_distribution.domain_name
    zone_id                = aws_cloudfront_distribution.fans_yourenicked_s3_distribution.hosted_zone_id
    evaluate_target_health = false
  }
} */

