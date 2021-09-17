#!/bin/bash

poolid="us-west-2:167b474a-b678-4271-8f1e-77f84aa530f7"
endpoint="a31gq30tvzx17m-ats.iot.us-west-2.amazonaws.com"
region="us-west-2"

if [[ -z "${REMOTE_CLIENT_ID}" ]]; then
    echo "Must set REMOTE_CLIENT_ID environment variable to mqtt client id of simulator or other publisher/subscriber of messages"
else
    if [[ -z "${COGNITO_ID}" ]]; then
        echo "Retrieving Cognito identity..."
        COGNITO_ID=$(aws cognito-identity get-id --identity-pool-id $poolid --region $region --output text)
    fi

    id_request=$(aws cognito-identity get-credentials-for-identity --identity-id "$COGNITO_ID" --region $region)

    export AWS_ACCESS_KEY_ID=$(echo "$id_request" | jq -r '.Credentials.AccessKeyId')
    export AWS_SECRET_ACCESS_KEY=$(echo "$id_request" | jq -r '.Credentials.SecretKey')
    export AWS_SESSION_TOKEN=$(echo "$id_request" | jq -r '.Credentials.SessionToken')

    $(dirname "$0")/build/access_control --endpoint $endpoint --region $region --remote-client-id "$REMOTE_CLIENT_ID"
fi
