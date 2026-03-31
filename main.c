#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ====================== DATA STRUCTURES ======================

// User profile
typedef struct {
    char name[50];
    int age;
    int gender;        // 1 = Male, 2 = Female
    float weight;      // in kg
    float height;      // in cm
    int activity_level; // 1 to 5
    float maintenance_calories;
} User;

// One day's log
typedef struct {
    char date[11];     // YYYY-MM-DD
    int calories_consumed;
} DailyLog;

// ====================== FUNCTION DECLARATIONS ======================

float calculate_bmr(User *user);
float calculate_tdee(float bmr, int activity_level);
void create_user_profile(User *user);
void show_daily_summary(User *user, int calories_today);
void save_user(User *user);
int load_user(User *user);
void save_log(int calories);
int get_calories_today();

// ====================== MAIN FUNCTION ======================

int main() {
    User user;
    int choice;
    int calories;

    printf("========================================\n");
    printf("     DAILY CALORIE TRACKER (CLI)\n");
    printf("========================================\n\n");

    // Try to load existing user, otherwise create new
    if (load_user(&user) == 0) {
        printf("No profile found. Let's create one.\n");
        create_user_profile(&user);
        save_user(&user);
    } else {
        printf("Welcome back, %s!\n", user.name);
    }

    while (1) {
        printf("\n--- Menu ---\n");
        printf("1. Log calories consumed today\n");
        printf("2. Show today's summary\n");
        printf("3. Show my maintenance calories\n");
        printf("4. Update my profile\n");
        printf("5. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("How many calories did you consume today? ");
                scanf("%d", &calories);
                save_log(calories);
                printf("Logged %d calories for today.\n", calories);
                break;

            case 2:
                show_daily_summary(&user, get_calories_today());
                break;

            case 3:
                printf("Your maintenance calories: %.0f kcal/day\n", user.maintenance_calories);
                break;

            case 4:
                create_user_profile(&user);
                save_user(&user);
                break;

            case 5:
                printf("Goodbye!\n");
                return 0;

            default:
                printf("Invalid option.\n");
        }
    }

    return 0;
}

// ====================== CALCULATION FUNCTIONS ======================

float calculate_bmr(User *user) {
    if (user->gender == 1) { // Male
        return 10 * user->weight + 6.25 * user->height - 5 * user->age + 5;
    } else { // Female
        return 10 * user->weight + 6.25 * user->height - 5 * user->age - 161;
    }
}

float calculate_tdee(float bmr, int activity_level) {
    float multipliers[] = {1.2, 1.375, 1.55, 1.725, 1.9};
    return bmr * multipliers[activity_level - 1];
}

// ====================== USER PROFILE FUNCTIONS ======================

void create_user_profile(User *user) {
    printf("\n=== Create Profile ===\n");
    printf("Enter your name: ");
    scanf(" %[^\n]", user->name);
    
    printf("Enter age: ");
    scanf("%d", &user->age);
    
    printf("Gender (1 = Male, 2 = Female): ");
    scanf("%d", &user->gender);
    
    printf("Weight (kg): ");
    scanf("%f", &user->weight);
    
    printf("Height (cm): ");
    scanf("%f", &user->height);
    
    printf("\nActivity Level:\n");
    printf("1. Sedentary (little to no exercise)\n");
    printf("2. Lightly Active (light exercise 1-3 days/week)\n");
    printf("3. Moderately Active (moderate exercise 3-5 days/week)\n");
    printf("4. Very Active (hard exercise 6-7 days/week)\n");
    printf("5. Super Active (very hard exercise + physical job)\n");
    printf("Choose (1-5): ");
    scanf("%d", &user->activity_level);

    float bmr = calculate_bmr(user);
    user->maintenance_calories = calculate_tdee(bmr, user->activity_level);

    printf("\nProfile created! Your maintenance calories: %.0f kcal/day\n", 
           user->maintenance_calories);
}

// ====================== FILE FUNCTIONS ======================

void save_user(User *user) {
    FILE *file = fopen("user.dat", "wb");
    if (file != NULL) {
        fwrite(user, sizeof(User), 1, file);
        fclose(file);
    }
}

int load_user(User *user) {
    FILE *file = fopen("user.dat", "rb");
    if (file == NULL) return 0;
    
    fread(user, sizeof(User), 1, file);
    fclose(file);
    return 1;
}

void save_log(int calories) {
    FILE *file = fopen("today.log", "w");
    if (file != NULL) {
        fprintf(file, "%d", calories);
        fclose(file);
    }
}

int get_calories_today() {
    FILE *file = fopen("today.log", "r");
    int calories = 0;
    if (file != NULL) {
        fscanf(file, "%d", &calories);
        fclose(file);
    }
    return calories;
}

void show_daily_summary(User *user, int calories_today) {
    int remaining = (int)user->maintenance_calories - calories_today;
    printf("\n=== Today's Summary ===\n");
    printf("Maintenance Calories : %.0f kcal\n", user->maintenance_calories);
    printf("Calories Consumed    : %d kcal\n", calories_today);
    printf("Calories Remaining   : %d kcal\n", remaining);
    
    if (remaining > 0)
        printf("Status: In deficit (good for weight loss)\n");
    else
        printf("Status: In surplus\n");
}
