
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

typedef struct FileMeta {
    char filename[256];    
    ino_t inodeNum;       
    time_t lastUpdated;     
} FileMeta;

typedef struct ListNode {
    FileMeta metadata;
    struct ListNode *next;
} ListNode;

ListNode* createListNode(FileMeta data) {
    ListNode *node = (ListNode*)malloc(sizeof(ListNode));
    node->metadata = data;
    node->next = NULL;
    return node;
}

void prependNode(ListNode **head, FileMeta data) {
    ListNode *node = createListNode(data);
    node->next = *head;
    *head = node;
}

ListNode* findNode(ListNode *head, const char *target) {
    while (head != NULL) {
        if (strcmp(head->metadata.filename, target) == 0) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

void removeNode(ListNode **head, const char *target) {
    ListNode *current = *head, *previous = NULL;

    while (current != NULL && strcmp(current->metadata.filename, target) == 0) {
        *head = current->next;
        free(current);
        return;
    }

    while (current != NULL && strcmp(current->metadata.filename, target) != 0) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) return;

    previous->next = current->next;
    free(current);
}

void printFileList(ListNode *head) {
    while (head != NULL) {
        printf("%s\n", head->metadata.filename);
        head = head->next;
    }
}

void loadDirectory(const char *dirPath, ListNode **head) {
    struct dirent *entry;
    struct stat fileStats;
    DIR *dir = opendir(dirPath);

    if (dir == NULL) {
        perror("Error: Cannot open the directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char pathBuffer[512];
        snprintf(pathBuffer, sizeof(pathBuffer), "%s/%s", dirPath, entry->d_name);

        if (stat(pathBuffer, &fileStats) == 0) {
            FileMeta meta;
            strncpy(meta.filename, entry->d_name, sizeof(meta.filename));
            meta.inodeNum = fileStats.st_ino;
            meta.lastUpdated = fileStats.st_mtime;

            prependNode(head, meta);
        }
    }

    closedir(dir);
}

void showFileDetails(FileMeta meta) {
    printf("File/Folder Name: %s\n", meta.filename);
    printf("Inode Number: %lu\n", meta.inodeNum);
    printf("Last Modified Time: %ld\n", meta.lastUpdated);
}

int main() {
    ListNode *fileList = NULL;
    int option;
    char inputPath[256], searchName[256];

    while (1) {
        printf("\nFile Manager Menu:\n");
        printf("1. Load Directory\n");
        printf("2. Show File List\n");
        printf("3. Show File Details\n");
        printf("4. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                printf("Enter directory path: ");
                scanf("%s", inputPath);
                while (fileList != NULL) {
                    ListNode *temp = fileList;
                    fileList = fileList->next;
                    free(temp);
                }
                loadDirectory(inputPath, &fileList);
                break;

            case 2:
                printf("Files and Folders:\n");
                printFileList(fileList);
                break;

            case 3:
                printf("Enter file/folder name: ");
                scanf("%s", searchName);
                ListNode *found = findNode(fileList, searchName);
                if (found) {
                    showFileDetails(found->metadata);
                } else {
                    printf("File or folder not found.\n");
                }
                break;

            case 4:
                printf("Exiting File Manager.\n");
                while (fileList != NULL) {
                    ListNode *temp = fileList;
                    fileList = fileList->next;
                    free(temp);
                }
                return 0;

            default:
                printf("Invalid option. Please choose again.\n");
        }
    }

    return 0;
}
