#include <stdio.h>
// অক্ষরকে বড়হাতের অক্ষরে রূপান্তর করে
char toUpper(char ch) {
    if (ch >= 'a' && ch <= 'z')
        return ch - 32; // ছোট হাতের হলে বড় হাতের অক্ষরে পরিবর্তন
    return ch;
}
// অক্ষরটি বর্ণমালা কিনা যাচাই করে
int isAlpha(char ch) {
    if ((ch >= 'A' && ch <= 'Z'))
        return 1;
    if ((ch >= 'a' && ch <= 'z'))
        return 1;
    return 0;
}
// কী টেবিল তৈরি করে (Playfair Key Table)
void generateKeyTable(char key[], char keyTable[5][5]) {
    int i, j, k = 0;
    char used[26] = {0};  // ব্যবহৃত অক্ষর ট্র্যাক করে
    char current;

    // কী স্ট্রিং থেকে টেবিল পূরণ করা
    for (i = 0; key[i] != '\0'; i++) {
        current = toUpper(key[i]); // বড় হাতের অক্ষর করা
        if (current == 'J') current = 'I'; // 'J' কে 'I' হিসেবে ধরা হয়
        if (isAlpha(current) && !used[current - 'A']) {
            keyTable[k / 5][k % 5] = current; // কী টেবিলে বসানো
            used[current - 'A'] = 1;
            k++;
        }
    }

    // বাকি অক্ষর পূরণ করা
    for (i = 0; i < 26; i++) {
        if (i + 'A' == 'J') continue; // 'J' বাদ
        if (!used[i]) {
            keyTable[k / 5][k % 5] = i + 'A';
            k++;
        }
    }
}
// কী টেবিল প্রিন্ট করে
void printKeyTable(char keyTable[5][5]) {
    int i, j;
    printf("\nPlayfair Key Table:\n");
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++)
            printf("%c ", keyTable[i][j]);
        printf("\n");
    }
}

// অক্ষরের অবস্থান খুঁজে বের করে কী টেবিলে
void findPosition(char keyTable[5][5], char ch, int *row, int *col) {
    int i, j;
    if (ch == 'J') ch = 'I'; // J কে I হিসেবে ধরা হয়
    for (i = 0; i < 5; i++)
        for (j = 0; j < 5; j++)
            if (keyTable[i][j] == ch) {
                *row = i;
                *col = j;
                return;
            }
}

// প্লেইন টেক্সট প্রস্তুত করা (বড় হাতের অক্ষরে)
void prepareText(char text[], char prepared[]) {
    int i, j = 0;
    for (i = 0; text[i] != '\0'; i++)
        if (isAlpha(text[i]))
            prepared[j++] = toUpper(text[i]);
    prepared[j] = '\0';
}

// টেক্সটকে দুই অক্ষরের জোড়ায় ভাগ করা
int makePairs(char prepared[], char pairs[][2]) {
    int i = 0, j = 0;
    while (prepared[i] != '\0') {
        char a = prepared[i];
        char b;
        if (prepared[i + 1] == '\0') {
            b = 'X'; // একক অক্ষর হলে X যোগ করা
            i++;
        } else {
            b = prepared[i + 1];
            if (a == b) {
                b = 'X'; // একই অক্ষর হলে মাঝখানে X বসানো
                i++;
            } else {
                i += 2;
            }
        }
        pairs[j][0] = a;
        pairs[j][1] = b;
        j++;
    }
    return j; // মোট কয়টি জোড়া তৈরি হয়েছে
}
// এনক্রিপশন প্রক্রিয়া
void encrypt(char pairs[][2], int pairCount, char keyTable[5][5], char cipher[]) {
    int i, row1, col1, row2, col2, k = 0;
    for (i = 0; i < pairCount; i++) {
        findPosition(keyTable, pairs[i][0], &row1, &col1);
        findPosition(keyTable, pairs[i][1], &row2, &col2);

        // যদি একই সারিতে থাকে
        if (row1 == row2) {
            cipher[k++] = keyTable[row1][(col1 + 1) % 5];
            cipher[k++] = keyTable[row2][(col2 + 1) % 5];
        }
        // যদি একই কলামে থাকে
        else if (col1 == col2) {
            cipher[k++] = keyTable[(row1 + 1) % 5][col1];
            cipher[k++] = keyTable[(row2 + 1) % 5][col2];
        }
        // ভিন্ন সারি ও কলাম হলে
        else {
            cipher[k++] = keyTable[row1][col2];
            cipher[k++] = keyTable[row2][col1];
        }
    }
    cipher[k] = '\0';
}
// ডিক্রিপশন প্রক্রিয়া
void decrypt(char pairs[][2], int pairCount, char keyTable[5][5], char plain[]) {
    int i, row1, col1, row2, col2, k = 0;
    for (i = 0; i < pairCount; i++) {
        findPosition(keyTable, pairs[i][0], &row1, &col1);
        findPosition(keyTable, pairs[i][1], &row2, &col2);

        // যদি একই সারিতে থাকে
        if (row1 == row2) {
            plain[k++] = keyTable[row1][(col1 + 5 - 1) % 5];
            plain[k++] = keyTable[row2][(col2 + 5 - 1) % 5];
        }
        // যদি একই কলামে থাকে
        else if (col1 == col2) {
            plain[k++] = keyTable[(row1 + 5 - 1) % 5][col1];
            plain[k++] = keyTable[(row2 + 5 - 1) % 5][col2];
        }
        // ভিন্ন সারি ও কলাম হলে
        else {
            plain[k++] = keyTable[row1][col2];
            plain[k++] = keyTable[row2][col1];
        }
    }
    plain[k] = '\0';
}
// মূল প্রোগ্রাম
int main() {
    char key[100], text[100], prepared[100], cipher[200], decrypted[200];
    char keyTable[5][5];
    char pairs[100][2];
    int pairCount;

    printf("Enter key: ");
    fgets(key, sizeof(key), stdin); // কী ইনপুট নেওয়া

    printf("Enter plaintext: ");
    fgets(text, sizeof(text), stdin); // প্লেইন টেক্সট ইনপুট নেওয়া

    generateKeyTable(key, keyTable); // কী টেবিল তৈরি করা
    printKeyTable(keyTable);         // কী টেবিল দেখানো

    prepareText(text, prepared);     // টেক্সট প্রস্তুত করা
    pairCount = makePairs(prepared, pairs); // জোড়া তৈরি করা

    encrypt(pairs, pairCount, keyTable, cipher); // এনক্রিপশন
    printf("\nEncrypted text: %s\n", cipher);

    // ডিক্রিপশন করার জন্য নতুন করে জোড়া তৈরি
    pairCount = makePairs(cipher, pairs);
    decrypt(pairs, pairCount, keyTable, decrypted); // ডিক্রিপশন
    printf("Decrypted text: %s\n", decrypted);

    return 0;
}
