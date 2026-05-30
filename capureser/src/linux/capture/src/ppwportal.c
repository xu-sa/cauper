#include <stdio.h>
#include <libportal/portal.h>
#include "../include/sccap.h"
struct PortalContext{
    XdpPortal *portal;
    XdpSession *session;
    GMainLoop *loop;
    GError *error;
};

void session_start(GObject *source, GAsyncResult *res, gpointer data);
void session_create(GObject *source, GAsyncResult *res, gpointer data);
static PortalContext* portal_create() {
    PortalContext *ctx=(PortalContext*)malloc(sizeof(PortalContext));
    ctx->portal = xdp_portal_new();
    ctx->loop=g_main_loop_new(NULL,0);
    ctx->session = NULL;
    ctx->error=NULL;
    printf("Created a new portal context object\n");
    return ctx;
}

int portal_init(sccap_context* ctx) {
    ctx->ppwportal=portal_create();
    xdp_portal_create_screencast_session(
        ctx->ppwportal->portal,
        XDP_OUTPUT_MONITOR,
        XDP_SCREENCAST_FLAG_NONE,
        XDP_CURSOR_MODE_EMBEDDED,
        XDP_PERSIST_MODE_NONE,
        NULL,
        NULL,
        session_create,//first callback
        ctx
    );
    printf("Waiting for user to grant access...\n");    
    g_main_loop_run(ctx->ppwportal->loop);
    g_main_loop_unref(ctx->ppwportal->loop);
    if (ctx->ppwportal->error) {
        printf("screencast error: %s\n", ctx->ppwportal->error->message);
        g_error_free(ctx->ppwportal->error);
        return 0;
    }
    printf("Permited for capturing: node_id: %d fd: %d size: %dx%d\n",ctx->node_id,ctx->fd,ctx->info.width,ctx->info.height);
    return 1;
}

void portal_destroy(sccap_context *ctx) {
    if (ctx->ppwportal->session) g_object_unref(ctx->ppwportal->session);
    g_object_unref(ctx->ppwportal->portal);
    free(ctx->ppwportal);
    printf("Portal context obj Released\n");
}