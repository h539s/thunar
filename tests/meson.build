test_bins = [
  'test-resolve-symlink',
]

test_snapshot_common_sources = [
  'test-snapshot-main.c',
  'thunar-test-snapshots.c',
  'thunar-test-control.c',
]

test_cases = [
  'open',
  'file-menu',
]

xvfb_run = find_program('xvfb-run', required: false)
cage = find_program('cage', required: false)
dbus_run_session = find_program('dbus-run-session', required: false)
sh = find_program('sh', required: true)

# Base environment variables
base_test_env = {
  'USER': 'user',
  'HOME': meson.current_source_dir() / 'home_env',
  'GTK_THEME': 'Adwaita',
  'LIBGL_ALWAYS_SOFTWARE': '1',
  'GIO_USE_VFS': 'local',
  'GIO_USE_VOLUME_MONITOR': 'unix',
  'GIO_EXTRA_MODULES': '/tmp/empty_gio',
  'G_TEST_SRCDIR': meson.current_source_dir(),
}

foreach tc : test_cases
  tc_name = 'test-snapshot-' + tc
  e = executable(
    tc_name,
    sources: test_snapshot_common_sources + [tc_name + '.c'],
    c_args: thunar_c_args + ['-UG_DISABLE_ASSERT', '-DTHUNAR_TEST_SNAPSHOT'],
    include_directories: [
      include_directories('..'),
    ],
    dependencies: thunar_dependencies,
    link_with: [ libthunar, libthunarx ],
    install: false,
  )

  # X11 tests
  if xvfb_run.found() and dbus_run_session.found()
    x11_env = environment()
    foreach var, val : base_test_env
      x11_env.set(var, val)
    endforeach

    test(
      tc_name + '-x11',
      sh,
      args: [
        '-c',
        'mkdir -p /tmp/empty_gio && ' + xvfb_run.full_path() + ' --auto-servernum -s "-screen 0 800x600x24" ' + dbus_run_session.full_path() + ' -- ' + e.full_path()
      ],
      env: x11_env,
      timeout: 60,
      suite: 'snapshots',
    )
  endif

  # Wayland tests
  if cage.found() and dbus_run_session.found()
    wayland_env = environment()
    foreach var, val : base_test_env
      wayland_env.set(var, val)
    endforeach
    wayland_env.set('WLR_BACKENDS', 'headless')
    wayland_env.set('WLR_LIBINPUT_NO_DEVICES', '1')
    wayland_env.set('WLR_HEADLESS_OUTPUT_WIDTH', '800')
    wayland_env.set('WLR_HEADLESS_OUTPUT_HEIGHT', '600')
    wayland_env.set('XDG_RUNTIME_DIR', '/tmp')

    test(
      tc_name + '-wayland',
      sh,
      args: [
        '-c',
        'mkdir -p /tmp/empty_gio && ' + cage.full_path() + ' -- ' + dbus_run_session.full_path() + ' -- ' + e.full_path()
      ],
      env: wayland_env,
      timeout: 60,
      suite: 'snapshots',
    )
  endif
endforeach

foreach bin : test_bins
  e = executable(
    bin,
    sources: [
      '@0@.c'.format(bin),
    ],
    c_args: thunar_c_args + '-UG_DISABLE_ASSERT',
    include_directories: [
      include_directories('..'),
    ],
    dependencies: thunar_dependencies,
    link_with: [ libthunar, libthunarx ],
    install: false,
  )

  test(bin, e)
endforeach
