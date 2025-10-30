#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>


typedef enum {
    C_FILE =0,
    JS_FILE=1,
    H_FILE=2,
    CPP_FILE=3,
    TEXT_FILE=4,
    FOLDER=5,
    HTML_FILE=6,
    CSS_FILE=7,
    CONFIG_FILE=8,
    EXE_FILE=9

}FILE_TYPE;

char* typeToStr(FILE_TYPE type){
      
     switch(type){
      case 0 : return "C File"; 
      case 1 : return "JS File"; 
      case 2 : return "Header File"; 
      case 3 : return "CPP File"; 
      case 4 : return "Text File"; 
      case 5 : return "Folder"; 
      case 6 : return "Html File"; 
      case 7 : return "Css File"; 
      case 8 : return "Config File";
      case 9 : return "Executable"; 
      default : return "UNKNOWN";
     };
}

typedef struct metadata metadata;
struct metadata{
    char* filename;
    char* extension;
    char* buffer;
    char* description;
    size_t filesize;
    size_t buffer_size;
    FILE_TYPE ftype;
};
typedef struct tree tree;
 struct tree{
    void* data;
    tree* left;
    tree* right;

};
 
metadata* create_metadata(const char* filename,const char* ext,FILE_TYPE type,size_t fsize){
       
        metadata* meta = malloc(sizeof(metadata));
        if(!meta){
            return NULL;
        }
        meta->filename = strdup(filename);
        meta->extension = strdup(ext);
        meta->ftype = type;
        meta->filesize = fsize;
        meta->buffer_size = strlen(filename) + strlen(ext) + 128;
        meta->buffer = malloc(meta->buffer_size);
        if(!meta->buffer){
            free(meta);
            free(meta->filename);
            free(meta->extension);
            free(meta->description);
            return NULL;
        }
         snprintf(meta->buffer, meta->buffer_size,
        "{\n"
        "  \"filename\": \"%s\",\n"
        "  \"extension\": \"%s\",\n"
        "  \"type\": \"%s\",\n"
        "  \"size_bytes\": %zu\n"
        "}",
        filename, ext, typeToStr(type), fsize
    );
        
        return meta;
}

tree* init_tree(metadata* data){
     if(!data) return NULL;
    tree* root = malloc(sizeof(tree));
    if(!root) return NULL;   
    root->data  = data; 
    root->left = NULL;
    root->right = NULL;    
    return root;
}

void print_tree(tree* root){
    if(!root){
        return;
    }
    print_tree(root->left);
    printf("%s \n" ,((metadata*)root->data)->buffer);
    print_tree(root->right);
}

FILE_TYPE detect_file_type(const char* filename, int is_dir) {
    if (is_dir) return FOLDER;

    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) return TEXT_FILE;
    if (strcmp(dot, ".c") == 0) return C_FILE;
    if (strcmp(dot, ".js") == 0) return JS_FILE;
    if (strcmp(dot, ".h") == 0) return H_FILE;
    if (strcmp(dot, ".cpp") == 0) return CPP_FILE;
    if (strcmp(dot, ".html") == 0) return HTML_FILE;
    if (strcmp(dot, ".css") == 0) return CSS_FILE;
    if (strcmp(dot, ".cfg") == 0) return CONFIG_FILE;
    if (strcmp(dot, ".exe") == 0) return EXE_FILE;

    return TEXT_FILE;
}

// just print to the cmd doesnt construct a tree 
void gen_metadata_tree(const char* filepath){
 struct stat st;
 if(stat(filepath,&st)!=0){
    fprintf(stderr,"cannot stat file : %s",filepath);
    return;
 }
 if(S_ISDIR(st.st_mode)){
    DIR* dir = opendir(filepath);
    if (!dir){
        fprintf(stderr,"cannot open current directory %s \n",filepath);
         return;
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL){
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name,"..") == 0){
            continue;
        }
        char buffer[1024];
        snprintf(buffer,sizeof(buffer),"%s",entry->d_name);
        gen_metadata_tree(buffer);
    }
    closedir(dir);

   
    
 }else if ( S_ISREG(st.st_mode)){
    const char* dot = strrchr(filepath,'.');
    const char* ext = (dot && dot != filepath) ? dot + 1 : "";
    FILE_TYPE type = detect_file_type(filepath, 0);
    metadata* meta = create_metadata(filepath,ext,type,st.st_size);
    tree* root = init_tree(meta);
    print_tree(root);


    free(meta->buffer);
    free(meta->filename);
    free(meta->extension);
    free(meta->description);
    free(meta);
    free(root);
 }

}


tree* construct_metadata_tree(const char* filepath) {
    struct stat st;
    if(stat(filepath, &st) != 0){
        fprintf(stderr, "cannot stat file: %s\n", filepath);
        return NULL;
    }

    FILE_TYPE type = S_ISDIR(st.st_mode) ? FOLDER : detect_file_type(filepath, 0);


    const char* dot = strrchr(filepath, '.');
    const char* ext = (dot && dot != filepath) ? dot + 1 : "";
    metadata* meta = create_metadata(filepath, ext, type, st.st_size);
    tree* root = init_tree(meta);

    if(S_ISDIR(st.st_mode)) {
        DIR* dir = opendir(filepath);
        if(!dir){
            fprintf(stderr, "cannot open directory: %s\n", filepath);
            return root; 
        }

        struct dirent* entry;
        tree* last_child = NULL;

        while((entry = readdir(dir)) != NULL){
            if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            char child_path[1024];
            snprintf(child_path, sizeof(child_path), "%s/%s", filepath, entry->d_name);

            tree* child = construct_metadata_tree(child_path);
            if(!child) continue;

           
            if(!root->left) {
                root->left = child;   // first child
            } else {
                last_child->right = child;  // subsequent children
            }
            last_child = child;
        }
        closedir(dir);
    }

    return root;
}

// Print tree recursively
void rec_print_tree(tree* root, int level) {
    if(!root) return;

    for(int i = 0; i < level; i++) printf("  "); // indentation
    printf("%s\n", ((metadata*)root->data)->buffer);

    rec_print_tree(root->left, level + 1);  // print children
    rec_print_tree(root->right, level);     // print siblings
}

// Free tree recursively
void rec_free_tree(tree* root) {
    if(!root) return;
    rec_free_tree(root->left);
    rec_free_tree(root->right);

    metadata* m = (metadata*)root->data;
    free(m->buffer);
    free(m->filename);
    free(m->extension);
    free(m);
    free(root);
}

void tree_to_json(tree* root, FILE* out, int level) {
    if (!root) return;

    metadata* m = (metadata*)root->data;

    for (int i = 0; i < level; i++) fprintf(out, "  ");

    fprintf(out, "{\n");

    for (int i = 0; i < level + 1; i++) fprintf(out, "  ");
    fprintf(out, "\"filename\": \"%s\",\n", m->filename);

    for (int i = 0; i < level + 1; i++) fprintf(out, "  ");
    fprintf(out, "\"extension\": \"%s\",\n", m->extension);

    for (int i = 0; i < level + 1; i++) fprintf(out, "  ");
    fprintf(out, "\"type\": \"%s\",\n", typeToStr(m->ftype));

    for (int i = 0; i < level + 1; i++) fprintf(out, "  ");
    fprintf(out, "\"size_bytes\": %zu", m->filesize);

    // If folder, recursively print children
    if (m->ftype == FOLDER && root->left) {
        fprintf(out, ",\n");
        for (int i = 0; i < level + 1; i++) fprintf(out, "  ");
        fprintf(out, "\"children\": [\n");

        tree* child = root->left;
        while (child) {
            tree_to_json(child, out, level + 2);
            if (child->right) fprintf(out, ",\n");
            else fprintf(out, "\n");
            child = child->right;
        }

        for (int i = 0; i < level + 1; i++) fprintf(out, "  ");
        fprintf(out, "]\n");
    } else {
        fprintf(out, "\n");
    }

    for (int i = 0; i < level; i++) fprintf(out, "  ");
    fprintf(out, "}");
}

void print_tree_as_json(tree* root) {
    printf("[\n");
    tree* t = root;
    while (t) {
        tree_to_json(t, stdout, 1);
        if (t->right) printf(",\n");
        else printf("\n");
        t = t->right;
    }
    printf("]\n");
}

void export_tree_to_json_file(tree* root, const char* filename) {
    FILE* out = fopen(filename, "w");
    if (!out) {
        fprintf(stderr, "Failed to open file for writing: %s\n", filename);
        return;
    }

    fprintf(out, "[\n");
    tree* t = root;
    while (t) {
        tree_to_json(t, out, 1);
        if (t->right) fprintf(out, ",\n");
        else fprintf(out, "\n");
        t = t->right;
    }
    fprintf(out, "]\n");

    fclose(out);
    printf("JSON exported successfully to %s\n", filename);
}


int main(){
   // generate metadata tree for current path
    tree* root =  construct_metadata_tree("c:/zeus/cAlgos");
    //print_tree_as_json(root);
    export_tree_to_json_file(root,"metadata.json");
    rec_free_tree(root);

    return 0x1;

}


