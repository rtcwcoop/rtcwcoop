# Main binary
file (GLOB BZZWOLFSP_SOURCES
	src/sys/sys_main.c
	src/botlib/be_aas_bspq3.c
	src/botlib/be_aas_cluster.c
	src/botlib/be_aas_debug.c
	src/botlib/be_aas_entity.c
	src/botlib/be_aas_file.c
	src/botlib/be_aas_main.c
	src/botlib/be_aas_move.c
	src/botlib/be_aas_optimize.c
	src/botlib/be_aas_reach.c
	src/botlib/be_aas_route.c
	src/botlib/be_aas_routealt.c
	src/botlib/be_aas_routetable.c
	src/botlib/be_aas_sample.c
	src/botlib/be_ai_char.c
	src/botlib/be_ai_chat.c
	src/botlib/be_ai_gen.c
	src/botlib/be_ai_goal.c
	src/botlib/be_ai_move.c
	src/botlib/be_ai_weap.c
	src/botlib/be_ai_weight.c
	src/botlib/be_ea.c
	src/botlib/be_interface.c
	src/botlib/l_crc.c
	src/botlib/l_libvar.c
	src/botlib/l_log.c
	src/botlib/l_memory.c
	src/botlib/l_precomp.c
	src/botlib/l_script.c
	src/botlib/l_struct.c
	src/client/cl_cgame.c
	src/client/cl_cin.c
	src/client/cl_console.c
	src/client/cl_input.c
	src/client/cl_keys.c
	src/client/cl_main.c
	src/client/cl_net_chan.c
	src/client/cl_parse.c
	src/client/cl_scrn.c
	src/client/cl_ui.c
	src/client/snd_adpcm.c
	src/client/snd_dma.c
	src/client/snd_mem.c
	src/client/snd_mix.c
	src/client/snd_wavelet.c
	src/game/q_math.c
	src/game/q_shared.c
	src/jpeg-6/jcapimin.c
	src/jpeg-6/jccoefct.c
	src/jpeg-6/jccolor.c
	src/jpeg-6/jcdctmgr.c
	src/jpeg-6/jchuff.c
	src/jpeg-6/jcinit.c
	src/jpeg-6/jcmainct.c
	src/jpeg-6/jcmarker.c
	src/jpeg-6/jcmaster.c
	src/jpeg-6/jcomapi.c
	src/jpeg-6/jcparam.c
	src/jpeg-6/jcphuff.c
	src/jpeg-6/jcprepct.c
	src/jpeg-6/jcsample.c
	src/jpeg-6/jctrans.c
	src/jpeg-6/jdapimin.c
	src/jpeg-6/jdapistd.c
	src/jpeg-6/jdatadst.c
	src/jpeg-6/jdatasrc.c
	src/jpeg-6/jdcoefct.c
	src/jpeg-6/jdcolor.c
	src/jpeg-6/jddctmgr.c
	src/jpeg-6/jdhuff.c
	src/jpeg-6/jdinput.c
	src/jpeg-6/jdmainct.c
	src/jpeg-6/jdmarker.c
	src/jpeg-6/jdmaster.c
	src/jpeg-6/jdpostct.c
	src/jpeg-6/jdsample.c
	src/jpeg-6/jdtrans.c
	src/jpeg-6/jerror.c
	src/jpeg-6/jfdctflt.c
	src/jpeg-6/jidctflt.c
	src/jpeg-6/jmemmgr.c
	src/jpeg-6/jmemnobs.c
	src/jpeg-6/jutils.c
	src/qcommon/cmd.c
	src/qcommon/cm_load.c
	src/qcommon/cm_patch.c
	src/qcommon/cm_polylib.c
	src/qcommon/cm_test.c
	src/qcommon/cm_trace.c
	src/qcommon/common.c
	src/qcommon/cvar.c
	src/qcommon/files.c
	src/qcommon/huffman.c
	src/qcommon/md4.c
	src/qcommon/msg.c
	src/qcommon/net_chan.c
	src/qcommon/net_ip.c
	src/qcommon/unzip.c
	src/qcommon/vm.c
	src/renderer/tr_animation.c
	src/renderer/tr_backend.c
	src/renderer/tr_bsp.c
	src/renderer/tr_cmds.c
	src/renderer/tr_cmesh.c
	src/renderer/tr_curve.c
	src/renderer/tr_flares.c
	src/renderer/tr_font.c
	src/renderer/tr_image.c
	src/renderer/tr_init.c
	src/renderer/tr_light.c
	src/renderer/tr_main.c
	src/renderer/tr_marks.c
	src/renderer/tr_mesh.c
	src/renderer/tr_model.c
	src/renderer/tr_noise.c
	src/renderer/tr_scene.c
	src/renderer/tr_shade.c
	src/renderer/tr_shader.c
	src/renderer/tr_shade_calc.c
	src/renderer/tr_shadows.c
	src/renderer/tr_sky.c
	src/renderer/tr_surface.c
	src/renderer/tr_world.c
	src/sdl/sdl_gamma.c
	src/sdl/sdl_glimp.c
	src/sdl/sdl_input.c
	src/sdl/sdl_snd.c
	src/server/sv_bot.c
	src/server/sv_ccmds.c
	src/server/sv_client.c
	src/server/sv_game.c
	src/server/sv_init.c
	src/server/sv_main.c
	src/server/sv_net_chan.c
	src/server/sv_snapshot.c
	src/server/sv_world.c
	src/splines/math_angles.cpp
	src/splines/math_matrix.cpp
	src/splines/math_quaternion.cpp
	src/splines/math_vector.cpp
	src/splines/q_parse.cpp
	src/splines/splines.cpp
	src/splines/util_str.cpp
	src/sys/con_log.c
)

if (UNIX)
	set (BZZWOLFSP_SOURCES ${BZZWOLFSP_SOURCES} src/sys/sys_unix.c
	                                            src/sys/con_tty.c)
elseif (WIN32)
	set (BZZWOLFSP_SOURCES ${BZZWOLFSP_SOURCES} src/sys/sys_win32.c
	                                            src/sys/con_win32.c)
elseif (APPLE)
	set (BZZWOLFSP_SOURCES ${BZZWOLFSP_SOURCES} src/sys/sys_osx.m
	                                            src/sys/con_tty.c)
endif ()

set (BZZWOLFSP_INCLUDES
	src/botlib
	src/client
	src/jpeg-6
	src/qcommon
	src/renderer
	src/sdl
	src/SDL12/include
	src/server
	src/sys
)


# Dedicated server
file (GLOB SERVER_SOURCES
	src/sys/sys_main.c
	src/botlib/be_aas_bspq3.c
	src/botlib/be_aas_cluster.c
	src/botlib/be_aas_debug.c
	src/botlib/be_aas_entity.c
	src/botlib/be_aas_file.c
	src/botlib/be_aas_main.c
	src/botlib/be_aas_move.c
	src/botlib/be_aas_optimize.c
	src/botlib/be_aas_reach.c
	src/botlib/be_aas_route.c
	src/botlib/be_aas_routealt.c
	src/botlib/be_aas_routetable.c
	src/botlib/be_aas_sample.c
	src/botlib/be_ai_char.c
	src/botlib/be_ai_chat.c
	src/botlib/be_ai_gen.c
	src/botlib/be_ai_goal.c
	src/botlib/be_ai_move.c
	src/botlib/be_ai_weap.c
	src/botlib/be_ai_weight.c
	src/botlib/be_ea.c
	src/botlib/be_interface.c
	src/botlib/l_crc.c
	src/botlib/l_libvar.c
	src/botlib/l_log.c
	src/botlib/l_memory.c
	src/botlib/l_precomp.c
	src/botlib/l_script.c
	src/botlib/l_struct.c
	src/game/q_math.c
	src/game/q_shared.c
	src/null/null_client.c
	src/null/null_input.c
	src/null/null_snddma.c
	src/qcommon/cmd.c
	src/qcommon/cm_load.c
	src/qcommon/cm_patch.c
	src/qcommon/cm_polylib.c
	src/qcommon/cm_test.c
	src/qcommon/cm_trace.c
	src/qcommon/common.c
	src/qcommon/cvar.c
	src/qcommon/files.c
	src/qcommon/huffman.c
	src/qcommon/md4.c
	src/qcommon/msg.c
	src/qcommon/net_chan.c
	src/qcommon/net_ip.c
	src/qcommon/unzip.c
	src/qcommon/vm.c
	src/server/sv_bot.c
	src/server/sv_ccmds.c
	src/server/sv_client.c
	src/server/sv_game.c
	src/server/sv_init.c
	src/server/sv_main.c
	src/server/sv_net_chan.c
	src/server/sv_snapshot.c
	src/server/sv_world.c
	src/sys/con_log.c
)

if (UNIX)
	set (SERVER_SOURCES ${SERVER_SOURCES} src/sys/sys_unix.c
	                                      src/sys/con_tty.c)
elseif (WIN32)
	set (SERVER_SOURCES ${SERVER_SOURCES} src/sys/sys_win32.c
	                                      src/sys/con_win32.c)
elseif (APPLE)
	set (SERVER_SOURCES ${SERVER_SOURCES} src/sys/sys_osx.m
	                                      src/sys/con_tty.c)
endif ()


# "cgame" library
file (GLOB CGAME_SOURCES
	src/cgame/cg_main.c
	src/cgame/cg_consolecmds.c
	src/cgame/cg_draw.c
	src/cgame/cg_drawtools.c
	src/cgame/cg_effects.c
	src/cgame/cg_ents.c
	src/cgame/cg_event.c
	src/cgame/cg_flamethrower.c
	src/cgame/cg_info.c
	src/cgame/cg_localents.c
	src/cgame/cg_marks.c
	src/cgame/cg_newDraw.c
	src/cgame/cg_omnibot.c
	src/cgame/cg_particles.c
	src/cgame/cg_players.c
	src/cgame/cg_playerstate.c
	src/cgame/cg_predict.c
	src/cgame/cg_scoreboard.c
	src/cgame/cg_servercmds.c
	src/cgame/cg_snapshot.c
	src/cgame/cg_sound.c
	src/cgame/cg_syscalls.c
	src/cgame/cg_trails.c
	src/cgame/cg_view.c
	src/cgame/cg_weapons.c
	src/game/bg_animation.c
	src/game/bg_misc.c
	src/game/bg_pmove.c
	src/game/bg_slidemove.c
	src/game/q_math.c
	src/game/q_shared.c
	src/ui/ui_shared.c
)

set (CGAME_INCLUDES
	src/cgame
)


# "qagame" library
file (GLOB QAGAME_SOURCES
	src/game/g_main.c
	src/botai/ai_chat.c
	src/botai/ai_cmd.c
	src/botai/ai_dmnet.c
	src/botai/ai_dmq3.c
	src/botai/ai_main.c
	src/botai/ai_team.c
	src/game/ai_cast.c
	src/game/ai_cast_characters.c
	src/game/ai_cast_debug.c
	src/game/ai_cast_events.c
	src/game/ai_cast_fight.c
	src/game/ai_cast_funcs.c
	src/game/ai_cast_func_attack.c
	src/game/ai_cast_func_boss1.c
	src/game/ai_cast_script.c
	src/game/ai_cast_script_actions.c
	src/game/ai_cast_script_ents.c
	src/game/ai_cast_sight.c
	src/game/ai_cast_think.c
	src/game/bg_animation.c
	src/game/bg_misc.c
	src/game/bg_pmove.c
	src/game/bg_slidemove.c
	src/game/g_active.c
	src/game/g_alarm.c
	src/game/g_antilag.c
	src/game/g_bot.c
	src/game/g_client.c
	src/game/g_cmds.c
	src/game/g_combat.c
	src/game/g_coop.c
	src/game/g_items.c
	src/game/g_mem.c
	src/game/g_misc.c
	src/game/g_missile.c
	src/game/g_mover.c
	src/game/g_props.c
	src/game/g_save.c
	src/game/g_script.c
	src/game/g_script_actions.c
	src/game/g_session.c
	src/game/g_spawn.c
	src/game/g_svcmds.c
	src/game/g_syscalls.c
	src/game/g_target.c
	src/game/g_team.c
	src/game/g_tramcar.c
	src/game/g_trigger.c
	src/game/g_utils.c
	src/game/g_weapon.c
	src/game/q_math.c
	src/game/q_shared.c
)

set (QAGAME_INCLUDES
	src/botai
	src/game
)


# "ui" library
file (GLOB UI_SOURCES
	src/ui/ui_main.c
	src/game/bg_animation.c
	src/game/bg_misc.c
	src/game/q_math.c
	src/game/q_shared.c
	src/ui/ui_atoms.c
	src/ui/ui_gameinfo.c
	src/ui/ui_players.c
	src/ui/ui_shared.c
	src/ui/ui_syscalls.c
	src/ui/ui_util.c
)

set (UI_INCLUDES
	src/ui
)
