/* resource "aws_s3_bucket" "fans_bucket" {
  bucket = var.fans-site-bucket-name
  acl    = "public-read"

  website {
    index_document = var.fans-index-file
  }

  tags = {
    project = var.tags_project
  }
}

resource "aws_s3_bucket_object" "fans_index" {
  bucket        = aws_s3_bucket.fans_bucket.bucket 
  source        = var.fans-index-file
  key           = var.fans-index-file
  content_type  = "text/html"
  acl           = "public-read"
  etag = filemd5(var.fans-index-file)
} */
