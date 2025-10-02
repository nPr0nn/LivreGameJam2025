
#include <sys/types.h>
#define SLC_IMPL
#define SLC_NO_LIB_PREFIX
#include "vendor/slc.h"
#include <stdio.h>

void build_vendors(String target_folder_path, bool build_to_web,
                   MemArena *arena_ptr) {

  String raylib_folder_path = string_from_cstr("./vendor/raylib/", arena_ptr);
  String c_compiler = string_from_cstr("gcc", arena_ptr);

  String raylib_modules[] = {
      string_from_cstr("rcore", arena_ptr),
      string_from_cstr("raudio", arena_ptr),
      string_from_cstr("rshapes", arena_ptr),
      // string_from_cstr("rmodels", arena_ptr),
      string_from_cstr("rtext", arena_ptr),
      string_from_cstr("rtextures", arena_ptr),
      string_from_cstr("utils", arena_ptr),
  };
  int num_raylib_modules = stack_array_size(raylib_modules);

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
  };
  int num_desktop_flags_list = stack_array_size(desktop_flags_list);

  // Array to store the paths of all generated object files.
  DynamicArray(String) object_files =
      dynamic_array_create(String, 16, arena_ptr);

  // Compile each raylib module
  for (int i = 0; i < num_raylib_modules; i++) {
    DynamicArray(String) args = dynamic_array_create(String, 32, arena_ptr);

    // 1. Add the compiler command
    dynamic_array_push_back(&args, c_compiler);

    // 2. Add all the compiler flags, each as a separate argument
    for (int j = 0; j < num_desktop_flags_list; j++) {
      dynamic_array_push_back(&args, desktop_flags_list[j]);
    }

    // 3. Add the "-c" flag
    dynamic_array_push_back(&args, string_from_cstr("-c", arena_ptr));

    // 4. Build and add the source file path
    String source_file =
        string_from_view(string_view(&raylib_folder_path), arena_ptr);
    string_append_view(&source_file, string_view(&raylib_modules[i]));
    string_append_cstr(&source_file, ".c");
    dynamic_array_push_back(&args, source_file);

    // 5. Add the "-o" flag
    dynamic_array_push_back(&args, string_from_cstr("-o", arena_ptr));

    // 6. Build and add the object file path
    String object_file =
        string_from_view(string_view(&target_folder_path), arena_ptr);
    string_append_view(&object_file, string_view(&raylib_modules[i]));
    string_append_cstr(&object_file, ".o");
    dynamic_array_push_back(&args, object_file);

    // Keep track of the object file for the final linking step.
    dynamic_array_push_back(&object_files, object_file);

    printf("Executing command: ");
    for (size_t k = 0; k < args.size; k++) {
      printf("%s ", args.data[k].data);
    }
    printf("\n");

    // 7. Execute the command
    cmd_exec(args.size, args.data);
  }

  // After compiling all modules, link them into a static library.
  printf("Linking static library libraylib.a...\n");

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

  printf("Executing command: ");
  for (size_t k = 0; k < ar_args.size; k++) {
    printf("%s ", ar_args.data[k].data);
  }
  printf("\n");

  // 5. Execute the linking command
  cmd_exec(ar_args.size, ar_args.data);
}

void build_game(String build_folder_path, bool build_to_web,
                MemArena *arena_ptr) {
  printf("Building game executable...\n");

  DynamicArray(String) args = dynamic_array_create(String, 32, arena_ptr);

  // 1. Compiler
  dynamic_array_push_back(&args, string_from_cstr("gcc", arena_ptr));

  // 2. Flags
  dynamic_array_push_back(&args, string_from_cstr("-std=c99", arena_ptr));
  dynamic_array_push_back(&args, string_from_cstr("-Iraylib", arena_ptr));

  // 3. Output file
  String output_file =
      string_from_view(string_view(&build_folder_path), arena_ptr);
  string_append_cstr(&output_file, "game");
  dynamic_array_push_back(&args, string_from_cstr("-o", arena_ptr));
  dynamic_array_push_back(&args, output_file);

  // 4. Source file
  dynamic_array_push_back(&args, string_from_cstr("src/main.c", arena_ptr));

  // 5. Library Path
  String lib_path = string_from_cstr("-L", arena_ptr);
  string_append_view(&lib_path, string_view(&build_folder_path));
  dynamic_array_push_back(&args, lib_path);

  // 6. Libraries to link
  dynamic_array_push_back(&args, string_from_cstr("-lraylib", arena_ptr));
  dynamic_array_push_back(&args, string_from_cstr("-lm", arena_ptr));
  dynamic_array_push_back(&args, string_from_cstr("-pthread", arena_ptr));
  dynamic_array_push_back(&args, string_from_cstr("-ldl", arena_ptr));
  dynamic_array_push_back(&args, string_from_cstr("-lrt", arena_ptr));
  dynamic_array_push_back(&args, string_from_cstr("-lX11", arena_ptr));
  dynamic_array_push_back(&args, string_from_cstr("-lXrandr", arena_ptr));
  dynamic_array_push_back(&args, string_from_cstr("-lGL", arena_ptr));

  printf("Executing command: ");
  for (size_t k = 0; k < args.size; k++) {
    printf("%s ", args.data[k].data);
  }
  printf("\n");

  // Execute the final compilation command
  cmd_exec(args.size, args.data);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <command> [options]\n", argv[0]);
    fprintf(stderr, "Commands:\n");
    fprintf(stderr, "  vendors [web]   - Build vendor libraries\n");
    fprintf(stderr, "  game    [web]   - Build the game executable\n");
    return 1;
  }

  MemArena arena = {0};
  MemArena *arena_ptr = &arena;

  const char *command = argv[1];
  bool build_to_web = false;

  if (argc > 2 && strcmp(argv[2], "web") == 0) {
    build_to_web = true;
  }

  String build_folder = string_from_cstr("target/", arena_ptr);

  if (strcmp(command, "vendors") == 0) {
    build_vendors(build_folder, build_to_web, arena_ptr);
  } else if (strcmp(command, "game") == 0) {
    build_game(build_folder, build_to_web, arena_ptr);
  } else {
    fprintf(stderr, "Unknown command: %s\n", command);
    mem_arena_free(&arena);
    return 1;
  }

  mem_arena_free(&arena);
  return 0;
}
