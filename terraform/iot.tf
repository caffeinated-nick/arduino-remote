resource "aws_iot_thing" "arduino_remote" {
  name = "arduino-remote"
}

/* resource "aws_iot_certificate" "cert" {
  active = true
} */

resource "aws_iot_policy" "pubsub" {
  name = "PubSubToAnyTopic"
  policy = jsonencode({
    Version = "2012-10-17"
    Statement = [
      {
        Action = [
          "iot:Connect",
        ]
        Effect   = "Allow"
        Resource = "arn:aws:iot:${var.aws_region}:${var.aws_account_id}:client/${aws_iot_thing.arduino_remote.name}"
      },
      {
        Action = [
          "iot:Subscribe",
        ]
        Effect   = "Allow"
        Resource = "arn:aws:iot:${var.aws_region}:${var.aws_account_id}:client/${aws_iot_thing.arduino_remote.name}"
      },
      {
        Action = [
          "iot:Receive",
        ]
        Effect   = "Allow"
        Resource = "arn:aws:iot:${var.aws_region}:${var.aws_account_id}:client/${aws_iot_thing.arduino_remote.name}"
      },
      {
        Action = [
          "iot:Publish",
        ]
        Effect   = "Allow"
        Resource = "arn:aws:iot:${var.aws_region}:${var.aws_account_id}:client/${aws_iot_thing.arduino_remote.name}"
      }
    ]
  })
}

resource "aws_iot_policy_attachment" "att" {
  policy = aws_iot_policy.pubsub.name
  /* target = aws_iot_certificate.cert.arn */
  target = "arn:aws:iot:ap-southeast-2:051340822516:cert/596b7baeea771f20969e9baf179fe178db2a4652a02de117de781b47e2519915"
}
