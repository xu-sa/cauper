#include "../include/sccap.h"
#include <gio/gio.h>
#include <stdio.h>
#include <libportal/portal.h>

// void session_start(GObject *source, GAsyncResult *res, gpointer data);
// void session_create(GObject *source, GAsyncResult *res, gpointer data);

struct PortalContext{
    XdpPortal *portal;
    XdpSession *session;
    GMainLoop *loop;
    GError *error;
};
inline static int iteration1(GVariantIter* iter,sccap_context* ctx){
    GVariant *v1;  
    GVariant *v2;
    if ((v1 = g_variant_iter_next_value(iter)) != NULL) {
        guint32 stream_id;
        g_variant_get(v1, "(u@a{sv})", &stream_id, &v2);
        if (stream_id > 0) {
            ctx->node_id=stream_id;
            GVariant *size_val = g_variant_lookup_value(v2, "size", NULL);
            if (size_val) {
                gint32 w, h;
                g_variant_get(size_val, "(ii)", &ctx->info.width, &ctx->info.height);
                g_variant_unref(size_val);
                g_variant_unref(v2);
                g_variant_unref(v1);
                return 0;
            }
            g_variant_unref(v2);
            g_variant_unref(v1);
            return 0;
        }
        g_variant_unref(v2);
        g_variant_unref(v1);
        return 1;
    }
    return 1;
}
static int parse_data(sccap_context* ctx){
    GVariant *v0 = xdp_session_get_streams(ctx->ppwportal->session);
    if (v0) {
        GVariantIter iter;
        g_variant_iter_init(&iter, v0);
        // printf("got stream data : %s\n",g_variant_print(v0,TRUE));
        if(iteration1(&iter,ctx)){
            g_variant_unref(v0);
            return 1;
        }
        g_variant_unref(v0);
        return 0;
    }
    return 1;
}

void session_start(GObject *source, GAsyncResult *res, gpointer data) {
    sccap_context *r_ = (sccap_context *)data;
    PortalContext* rd=r_->ppwportal;
    gboolean ok = xdp_session_start_finish(rd->session, res, &rd->error);
    
    if (!ok) {
        g_main_loop_quit(rd->loop);
        return;
    }
    
    if ((r_->fd=xdp_session_open_pipewire_remote(rd->session))< 0) {//fd
        rd->error = g_error_new(1, 0, "session_start(): failed at xdp_session_open_pipewire_remote(), callback 2d");
        g_main_loop_quit(rd->loop);
        return;
    }
    
    if(parse_data(r_)){//width height node id 
        rd->error = g_error_new(1, 0, "session_start(): failed when parsing stream data\n");
        g_main_loop_quit(rd->loop);
        return;
    }

    g_main_loop_quit(rd->loop);//Succeed
}

void session_create(GObject *source, GAsyncResult *res, gpointer data) {
    PortalContext *rd = ((sccap_context *)data)->ppwportal;
    XdpSession *session = xdp_portal_create_screencast_session_finish(XDP_PORTAL(source), res, &rd->error);
    if (!session) {
        g_main_loop_quit(rd->loop);
        return;
    }
    rd->session = session;
    xdp_session_start(session, NULL, NULL, session_start, data);//second callback
}
