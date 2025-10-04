
#define SLC_IMPL
#define SLC_NO_LIB_PREFIX
#include "vendor/slc.h"
#include <stdio.h>

void build_vendors(String target_folder_path, bool build_to_web,
                   MemArena *arena_ptr) {

  String raylib_folder_path = string_from_cstr("./vendor/raylib/", arena_ptr);

  String compiler_name;
  if (build_to_web)
    compiler_name = string_from_cstr("emcc", arena_ptr);
  else
    compiler_name = string_from_cstr("gcc", arena_ptr);

  String raylib_modules[] = {string_from_cstr("rcore", arena_ptr),
                             string_from_cstr("raudio", arena_ptr),
                             string_from_cstr("rshapes", arena_ptr),
                             // string_from_cstr("rmodels", arena_ptr),
                             string_from_cstr("rtext", arena_ptr),
                             string_from_cstr("rtextures", arena_ptr),
                             string_from_cstr("utils", arena_ptr),

                            //  string_from_cstr("rglfw", arena_ptr)
                            };
  i32 num_raylib_modules = stack_array_size(raylib_modules);

  String *flags_list;
  i32 flags_count = -1;
  String desktop_flags_list[] = {
      string_from_cstr("-Wall", arena_ptr),
      string_from_cstr("-D_GNU_SOURCE", arena_ptr),
      string_from_cstr("-DPLATFORM_DESKTOP_RGFW", arena_ptr),
      string_from_cstr("-DGRAPHICS_API_OPENGL_33", arena_ptr),
      string_from_cstr("-Wno-missing-braces", arena_ptr),
      string_from_cstr("-Werror=pointer-arith", arena_ptr),
      string_from_cstr("-fno-strict-aliasing", arena_ptr),
      string_from_cstr("-std=c99", arena_ptr),
      string_from_cstr("-fPIC", arena_ptr),
      string_from_cstr("-O2", arena_ptr),
      string_from_cstr("-Werror=implicit-function-declaration", arena_ptr),
      // string_from_cstr("-D_GLFW_X11", arena_ptr),
      // string_from_cstr("-I.", arena_ptr),
      // string_from_cstr("-Ivendor/raylib/external/glfw/include", arena_ptr)
      };
  i32 desktop_flags_count = stack_array_size(desktop_flags_list);

  String web_flags_list[] = {
      string_from_cstr("-Wall", arena_ptr),
      string_from_cstr("-DPLATFORM_WEB_RGFW", arena_ptr),
      string_from_cstr("-DGRAPHICS_API_OPENGL_ES2", arena_ptr),
  };
  i32 web_flags_count = stack_array_size(web_flags_list);

  if (build_to_web) {
    flags_list = web_flags_list;
    flags_count = web_flags_count;
  } else {
    flags_list = desktop_flags_list;
    flags_count = desktop_flags_count;
  }

  // Array to store the paths of all generated object files.
  DynamicArray(String) object_files =
      dynamic_array_create(String, 16, arena_ptr);

  // Compile each raylib module
  for (int i = 0; i < num_raylib_modules; i++) {
    DynamicArray(String) args = dynamic_array_create(String, 32, arena_ptr);

    // 1. Add the compiler command
    dynamic_array_push_back(&args, compiler_name);

    // 2. Add all the compiler flags, each as a separate argument
    for (int j = 0; j < flags_count; j++) {
      dynamic_array_push_back(&args, flags_list[j]);
    }

    // 3. Add the "-c" flag
    dynamic_array_push_back(&args, string_from_cstr("-c", arena_ptr));

    // 4. Build and add the source file path
    String source_file =
        string_from_view(string_view(&raylib_folder_path), arena_ptr);
    string_append(&source_file, &raylib_modules[i]);
    string_append_cstr(&source_file, ".c");
    dynamic_array_push_back(&args, source_file);

    // 5. Add the "-o" flag
    dynamic_array_push_back(&args, string_from_cstr("-o", arena_ptr));

    // 6. Build and add the object file path
    String object_file =
        string_from_view(string_view(&target_folder_path), arena_ptr);
    string_append(&object_file, &raylib_modules[i]);
    string_append_cstr(&object_file, ".o");
    dynamic_array_push_back(&args, object_file);

    // Keep track of the object file for the final linking step.
    dynamic_array_push_back(&object_files, object_file);

    print("Executing command: ");
    for (size_t k = 0; k < args.size; k++) {
      print("%s ", args.data[k].data);
    }
    print("\n");

    // 7. Execute the command
    cmd_exec(args.size, args.data);
  }

  // After compiling all modules, link them into a static library.
  print("Linking static library libraylib.a...\n");

  DynamicArray(String) ar_args = dynamic_array_create(String, 32, arena_ptr);

  // 1. Add the archiver command "ar"
  dynamic_array_push_back(&ar_args, string_from_cstr("ar", arena_ptr));

  // 2. Add archiver flags "rcs"
  dynamic_array_push_back(&ar_args, string_from_cstr("rcs", arena_ptr));

  // 3. Build and add the output library path
  String library_file =
      string_from_view(string_view(&target_folder_path), arena_ptr);
  string_append_cstr(&library_file, "libraylib.a");
  dynamic_array_push_back(&ar_args, library_file);

  // 4. Add all the object files as arguments
  for (size_t i = 0; i < object_files.size; i++) {
    dynamic_array_push_back(&ar_args, object_files.data[i]);
  }

  print("Executing command: ");
  for (size_t k = 0; k < ar_args.size; k++) {
    print("%s ", ar_args.data[k].data);
  }
  print("\n");

  // 5. Execute the linking command
  cmd_exec(ar_args.size, ar_args.data);
}

void build_game(String build_folder_path, String exec_name, bool build_to_web,
                MemArena *arena_ptr) {
  print("Building game...\n");

  // Output path
  String output_file = string_create(arena_ptr);
  string_append(&output_file, &build_folder_path);
  string_append(&output_file, &exec_name);

  if (build_to_web) {
    // --- Web build (emcc) ---
    String args[] = {
        string_from_cstr("emcc", arena_ptr),
        string_from_cstr("-o", arena_ptr),
        string_from_cstr("target/web/index.js", arena_ptr),

        string_from_cstr("src/main.c", arena_ptr),
        string_from_cstr("src/game.c", arena_ptr),
        string_from_cstr("src/character.c", arena_ptr),

        string_from_cstr("-Os", arena_ptr),
        string_from_cstr("-Wall", arena_ptr),

        string_from_cstr("target/web/libraylib.a", arena_ptr),

        string_from_cstr("-s", arena_ptr),
        string_from_cstr("USE_GLFW=3", arena_ptr),

        string_from_cstr("-s", arena_ptr),
        string_from_cstr("FULL_ES2=1", arena_ptr),

        string_from_cstr("-DPLATFORM_WEB", arena_ptr),
    };
  } else {
    // --- Desktop build (gcc) ---
    String args[] = {
        string_from_cstr("gcc", arena_ptr),
        string_from_cstr("-std=c99", arena_ptr),
        string_from_cstr("-Iraylib", arena_ptr),
        string_from_cstr("-o", arena_ptr),
        output_file,

        string_from_cstr("src/main.c", arena_ptr),
        string_from_cstr("src/game.c", arena_ptr),
        string_from_cstr("src/character.c", arena_ptr),

        string_from_cstr("-L", arena_ptr),
        build_folder_path,

        string_from_cstr("-lraylib", arena_ptr),
        string_from_cstr("-lm", arena_ptr),
        string_from_cstr("-pthread", arena_ptr),
        string_from_cstr("-ldl", arena_ptr),
        string_from_cstr("-lrt", arena_ptr),
        string_from_cstr("-lX11", arena_ptr),
        string_from_cstr("-lXrandr", arena_ptr),
        string_from_cstr("-lGL", arena_ptr),
    };
    cmd_exec(stack_array_size(args), args);
  }
}

void run_game(String build_folder_path, String exec_name, bool build_to_web,
              MemArena *arena_ptr) {
  if (!build_to_web) {
    String command = string_create(arena_ptr);
    string_append_cstr(&command, "./");
    string_append(&command, &build_folder_path);
    string_append(&command, &exec_name);
    cmd_exec(1, &command);
  } else {
    String run_commands[] = {
        string_from_cstr("emrun", arena_ptr),
        string_from_cstr("index.html", arena_ptr),
    };
    i32 run_commands_count = stack_array_size(run_commands);
    cmd_exec(run_commands_count, run_commands);
  }
}

void help(const String *binary_name) {
  stream_print(stderr, "Usage: %s <command> [options]\n", binary_name->data);
  stream_print(stderr, "Commands:\n");
  stream_print(stderr, "  vendors [web] - Build vendor libraries\n");
  stream_print(stderr, "  game    [web] [run] - Build the game executable\n");
}

int main(int argc, char **argv) {
  MemArena arena = {0};
  MemArena *arena_ptr = &arena;

  if (argc < 1) {
    stream_print(stderr, "Internal error: no argv[0]\n");
    return 1;
  }

  String binary_name = string_from_cstr(argv[0], arena_ptr);
  if (argc < 2) {
    help(&binary_name);
    mem_arena_free(&arena);
    return 1;
  }

  String build_target = string_from_cstr(argv[1], arena_ptr);
  String build_folder = string_from_cstr("target/", arena_ptr);
  String executable_name = string_from_cstr("app", arena_ptr);

  bool should_build_vendors = string_equals_cstr(&build_target, "vendors");
  bool should_build_game = string_equals_cstr(&build_target, "game");

  bool build_to_web = false;
  bool should_run_game = false;

  // Handle first optional argument
  if (argc > 2) {
    String second_argument = string_from_cstr(argv[2], arena_ptr);
    if (string_equals_cstr(&second_argument, "web")) {
      build_to_web = true;
    } else if (string_equals_cstr(&second_argument, "run")) {
      should_run_game = true;
    } else {
      stream_print(stderr, "Unknown option: %s\n", second_argument.data);
      mem_arena_free(&arena);
      return 1;
    }
  }

  // Update the build folder
  if (build_to_web) {
    string_append_cstr(&build_folder, "web/");
  } else {
    string_append_cstr(&build_folder, "desktop/");
  }

  // Run Game after build
  if (argc > 3 && should_build_game) {
    String third_argument = string_from_cstr(argv[3], arena_ptr);
    if (string_equals_cstr(&third_argument, "run")) {
      should_run_game = true;
    } else {
      stream_print(stderr, "Unknown option: %s\n", third_argument.data);
      mem_arena_free(&arena);
      return 1;
    }
  }

  if (should_build_vendors) {
    stream_print(stdout, "[BUILD] Vendors -> %s (%s)\n", build_folder.data,
                 build_to_web ? "Web" : "Native");
    build_vendors(build_folder, build_to_web, arena_ptr);
  } else if (should_build_game) {
    stream_print(stdout, "[BUILD] Game -> %s (%s)\n", build_folder.data,
                 build_to_web ? "Web" : "Native");
    build_game(build_folder, executable_name, build_to_web, arena_ptr);
    if (should_run_game) {
      stream_print(stdout, "[RUN] Launching game...\n");
      run_game(build_folder, executable_name, build_to_web, arena_ptr);
    }

  } else {
    stream_print(stderr, "Unknown command: %s\n", build_target.data);
    help(&binary_name);
  }

  mem_arena_free(&arena);
  return 0;
}
