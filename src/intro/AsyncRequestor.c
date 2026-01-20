/** @example Intro/AsyncRequestor.c
 */

/*
 * AsyncRequestor
 *
 * This sample shows how to implement a Requestor for ASYNCHRONOUS Request-Reply messaging.
 * unlike BasicRequestor.c, this does NOT block waiting for a reply.
 *
 * Logic:
 * 1. Create a Session.
 * 2. Create a Temporary Topic for receiving replies.
 * 3. Subscribe to the Temporary Topic.
 * 4. Send a Request Message with:
 *    - ReplyTo: The Temporary Topic
 *    - CorrelationId: A unique ID to identify the request
 * 5. Continue processing (don't block).
 * 6. Handle the Reply asynchronously in the message receive callback.
 *
 * Copyright 2013-2019 Solace Corporation. All rights reserved. 
 */

#include "os.h"
#include "solclient/solClient.h"
#include "solclient/solClientMsg.h"
#include "common.h"
#include "RRcommon.h"

/* Global counter for received replies */
volatile int receivedCount = 0;
const int TOTAL_REQUESTS = 10;

/*****************************************************************************
 * Async Message Receive Callback
 * This function is called by the API thread when a message is received.
 * We handle the Reply message here.
 *****************************************************************************/
solClient_rxMsgCallback_returnCode_t
asyncMessageReceiveCallback ( solClient_opaqueSession_pt opaqueSession_p, solClient_opaqueMsg_pt msg_p, void *user_p )
{
    solClient_returnCode_t rc = SOLCLIENT_OK;
    solClient_opaqueContainer_pt replyStream_p;
    solClient_bool_t resultOk;
    double result;
    const char *correlationId;

    /* Check if the message has a Correlation ID */
    if ( solClient_msg_getCorrelationId ( msg_p, &correlationId ) == SOLCLIENT_OK ) {
        printf ( "Async Callback: Received Reply with Correlation ID: %s\n", correlationId );
    } else {
        printf ( "Async Callback: Received Reply (No Correlation ID)\n" );
    }

    /* Process the Binary Attachment (Body) */
    if ( ( rc = solClient_msg_getBinaryAttachmentStream ( msg_p, &replyStream_p ) ) == SOLCLIENT_OK ) {
        
        // 1. Get Success/Fail status
        if ( solClient_container_getBoolean ( replyStream_p, &resultOk, NULL ) == SOLCLIENT_OK ) {
            if ( resultOk ) {
                // 2. Get Result
                if ( solClient_container_getDouble ( replyStream_p, (double *)&result, NULL ) == SOLCLIENT_OK ) {
                    printf ( "   -> Result: %f\n", result );
                }
            } else {
                printf ( "   -> Operation Failed according to Replier.\n" );
            }
        }
    } else {
        // Not a valid reply format we expected
        printf ( "   -> Received message without expected binary stream.\n" );
    }

    /* Increment the received counter */
    receivedCount++;

    return SOLCLIENT_CALLBACK_OK;
}

/********************** Send Async Requests ***********************************
 * Send a request message using solClient_session_sendMsg (Non-blocking)
 ******************************************************************************/
static void
sendAsyncRequests ( solClient_opaqueSession_pt opaqueSession_p, const char *destinationName, const char *replyToAddress )
{
    solClient_returnCode_t rc;
    solClient_opaqueMsg_pt msg_p;
    solClient_destination_t destination;
    solClient_destination_t replyToDest;
    solClient_opaqueContainer_pt stream_p;
    RR_operation_t     operation;
    solClient_uint32_t operand1 = 9;
    solClient_uint32_t operand2 = 5;
    char correlationIdBuffer[32];
    int i;

    /* Allocate a message for requests. */
    if ( ( rc = solClient_msg_alloc ( &msg_p ) ) != SOLCLIENT_OK ) {
        common_handleError ( rc, "solClient_msg_alloc()" );
        return;
    }

    /* Set the ReplyTo destination (Where we want the answer) */
    replyToDest.destType = SOLCLIENT_TOPIC_TEMP_DESTINATION;
    replyToDest.dest = replyToAddress;
    if ( ( rc = solClient_msg_setReplyTo ( msg_p, &replyToDest, sizeof ( replyToDest ) ) ) != SOLCLIENT_OK ) {
        common_handleError ( rc, "solClient_msg_setReplyTo()" );
        goto freeMsg;
    }

    /* Set the Destination Topic (Where to send the request) */
    destination.destType = SOLCLIENT_TOPIC_DESTINATION;
    destination.dest = destinationName;
    if ( ( rc = solClient_msg_setDestination ( msg_p, &destination, sizeof ( destination ) ) ) != SOLCLIENT_OK ) {
        common_handleError ( rc, "solClient_msg_setDestination()" );
        goto freeMsg;
    }

    /* Loop through operations and send requests */
    for ( i = 0; i < TOTAL_REQUESTS; i++ ) {
        
        // Cycle through operations: 1, 2, 3, 4, 1, 2, ...
        operation = (RR_operation_t)((i % 4) + 1);

        printf ( "Main Thread: Sending Async Request %d/%d: %d %s %d\n", i+1, TOTAL_REQUESTS, operand1, RR_operationToString ( operation ), operand2 );

        /* Reset the message for reuse in next loop - CLEARS EVERYTHING including Dest & ReplyTo */
        if ( ( rc = solClient_msg_reset ( msg_p ) ) != SOLCLIENT_OK ) {
            common_handleError ( rc, "solClient_msg_reset()" );
            goto freeMsg;
        }

        /* Set the ReplyTo destination (Must set every time after reset) */
        if ( ( rc = solClient_msg_setReplyTo ( msg_p, &replyToDest, sizeof ( replyToDest ) ) ) != SOLCLIENT_OK ) {
            common_handleError ( rc, "solClient_msg_setReplyTo()" );
            goto freeMsg;
        }

        /* Set the Destination Topic (Must set every time after reset) */
        if ( ( rc = solClient_msg_setDestination ( msg_p, &destination, sizeof ( destination ) ) ) != SOLCLIENT_OK ) {
            common_handleError ( rc, "solClient_msg_setDestination()" );
            goto freeMsg;
        }

        /* Set Correlation ID */
        snprintf(correlationIdBuffer, sizeof(correlationIdBuffer), "REQ_%d", i);
        if ( ( rc = solClient_msg_setCorrelationId ( msg_p, correlationIdBuffer ) ) != SOLCLIENT_OK ) {
            common_handleError ( rc, "solClient_msg_setCorrelationId()" );
            goto freeMsg;
        }

        /* Create a stream in the binary attachment part */
        if ( ( rc = solClient_msg_createBinaryAttachmentStream ( msg_p, &stream_p, 100 ) ) != SOLCLIENT_OK ) {
            common_handleError ( rc, "solClient_msg_createBinaryAttachmentStream()" );
            goto freeMsg;
        }

        /* Put data into stream */
        solClient_container_addInt8 ( stream_p, ( solClient_int8_t ) operation, NULL );
        solClient_container_addInt32 ( stream_p, operand1, NULL );
        solClient_container_addInt32 ( stream_p, operand2, NULL );

        /* SEND NON-BLOCKING */
        if ( ( rc = solClient_session_sendMsg ( opaqueSession_p, msg_p ) ) != SOLCLIENT_OK ) {
            common_handleError ( rc, "solClient_session_sendMsg()" );
        }
    }

  freeMsg:
    solClient_msg_free ( &msg_p );
}


int
main ( int argc, char *argv[] )
{
    struct commonOptions commandOpts;
    solClient_returnCode_t rc = SOLCLIENT_OK;

    solClient_opaqueContext_pt context_p;
    solClient_context_createFuncInfo_t contextFuncInfo = SOLCLIENT_CONTEXT_CREATEFUNC_INITIALIZER;

    solClient_opaqueSession_pt session_p;
    solClient_session_createFuncInfo_t sessionFuncInfo = SOLCLIENT_SESSION_CREATEFUNC_INITIALIZER;
    const char     *sessionProps[50];
    int             propIndex = 0;

    char replyToTopic[SOLCLIENT_BUFINFO_MAX_TOPIC_SIZE + 1];

    printf ( "\nAsyncRequestor.c (Non-blocking version)\n" );

    /* Parse Options */
    common_initCommandOptions(&commandOpts, 
                               ( USER_PARAM_MASK | DEST_PARAM_MASK ), 
                               ( HOST_PARAM_MASK | PASS_PARAM_MASK | LOG_LEVEL_MASK | USE_GSS_MASK | ZIP_LEVEL_MASK));
    if ( common_parseCommandOptions ( argc, argv, &commandOpts, NULL ) == 0 ) {
        exit (1);
    }

    /* Initialize API */
    if ( ( rc = solClient_initialize ( SOLCLIENT_LOG_DEFAULT_FILTER, NULL ) ) != SOLCLIENT_OK ) {
        common_handleError ( rc, "solClient_initialize()" );
        goto cleanup;
    }
    common_printCCSMPversion (  );
    solClient_log_setFilterLevel ( SOLCLIENT_LOG_CATEGORY_ALL, commandOpts.logLevel );

    /* Create Context */
    if ( ( rc = solClient_context_create ( SOLCLIENT_CONTEXT_PROPS_DEFAULT_WITH_CREATE_THREAD,
                                           &context_p, &contextFuncInfo, sizeof ( contextFuncInfo ) ) ) != SOLCLIENT_OK ) {
        common_handleError ( rc, "solClient_context_create()" );
        goto cleanup;
    }

    /* Create Session - REGISTER OUR ASYNC CALLBACK HERE */
    sessionFuncInfo.rxMsgInfo.callback_p = asyncMessageReceiveCallback; // <--- Changed from common callback
    sessionFuncInfo.rxMsgInfo.user_p = NULL;
    sessionFuncInfo.eventInfo.callback_p = common_eventCallback;
    sessionFuncInfo.eventInfo.user_p = NULL;

    /* Session Properties (Same as before) */
    if ( commandOpts.targetHost[0] != (char) 0 ) {
        sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_HOST;
        sessionProps[propIndex++] = commandOpts.targetHost;
    }
    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_RECONNECT_RETRIES;
    sessionProps[propIndex++] = "3";
    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_COMPRESSION_LEVEL;
    sessionProps[propIndex++] = ( commandOpts.enableCompression ) ? "9" : "0";
    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_USERNAME;
    sessionProps[propIndex++] = commandOpts.username;
    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_PASSWORD;
    sessionProps[propIndex++] = commandOpts.password;
    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_SSL_VALIDATE_CERTIFICATE;
    sessionProps[propIndex++] = SOLCLIENT_PROP_DISABLE_VAL;
    sessionProps[propIndex] = NULL;

    if ( ( rc = solClient_session_create ( (char **) sessionProps, context_p, &session_p, &sessionFuncInfo, sizeof ( sessionFuncInfo ) ) ) != SOLCLIENT_OK ) {
        common_handleError ( rc, "solClient_session_create()" );
         goto cleanup;
    }

    if ( ( rc = solClient_session_connect ( session_p ) ) != SOLCLIENT_OK ) {
        common_handleError ( rc, "solClient_session_connect()" );
         goto cleanup;
    }

    /* 
     * SETUP ASYNC REPLY HANDLING 
     * 1. Create a temporary topic for replies 
     * 2. Subscribe to it
     */
    if ( ( rc = solClient_session_createTemporaryTopicName ( session_p, replyToTopic, sizeof ( replyToTopic ) ) ) != SOLCLIENT_OK ) {
        common_handleError ( rc, "solClient_session_createTemporaryTopicName()" );
        goto cleanup;
    }
    printf("Created Temporary Reply Topic: %s\n", replyToTopic);

    if ( ( rc = solClient_session_topicSubscribeExt ( session_p, 0, replyToTopic ) ) != SOLCLIENT_OK ) {
        common_handleError ( rc, "solClient_session_topicSubscribeExt()" );
        goto cleanup;
    }

    /* Send Requests Asynchronously */
    sendAsyncRequests ( session_p, commandOpts.destinationName, replyToTopic );

    /* 
     * Wait for replies. 
     * In a real app, you would be doing other work here. 
     */
    printf("Main Thread: Requests sent. Waiting for all %d replies...\n", TOTAL_REQUESTS);
    
    while ( receivedCount < TOTAL_REQUESTS ) {
        sleep(1);
        printf("Main Thread: Received %d/%d replies so far...\n", receivedCount, TOTAL_REQUESTS);
    }
    
    printf("Main Thread: All replies received. Exiting.\n");

    /*************************************************************************
     * CLEANUP
     *************************************************************************/
    solClient_session_disconnect ( session_p );

  cleanup:
    solClient_cleanup (  );
    return 0;
}