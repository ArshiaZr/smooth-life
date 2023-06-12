#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Constants
#define WIDTH 100
#define HEIGHT 100

char level_map[] = " .-coaA0#";
#define level_map_size (sizeof(level_map)/sizeof(level_map[0]) - 1)

float grid[HEIGHT][WIDTH] = {0};
float grid_diff[HEIGHT][WIDTH] = {0};

float r_a = 11.0f;
float b_1 = 0.278f;
float b_2 = 0.365f;
float d_1 = 0.267f; 
float d_2 = 0.445f;
float dt = 0.01f;
float alpha_n = 0.028, alpha_m = 0.147;


void display_grid(float grid[WIDTH][HEIGHT]){
  for(size_t i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      char c = level_map[(int)(grid[i][j] * (level_map_size - 1))];
      fputc(c, stdout);
      fputc(c, stdout);
    }
    fputc('\n', stdout);
  }
}

void print_grid(float grid[WIDTH][HEIGHT]){
  for(size_t i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      printf("%f ", grid[i][j]);
    }
    fputc('\n', stdout);
  }
}

float get_random_float(void){
    return (float)rand()/(float)RAND_MAX;
}

void initialize_random(void){
    size_t w = WIDTH/3;
    size_t h = HEIGHT/3;
    for (size_t dy = 0; dy < h; ++dy) {
        for (size_t dx = 0; dx < w; ++dx) {
            size_t x = dx + WIDTH/2 - w/2;
            size_t y = dy + HEIGHT/2 - h/2;
            grid[y][x] = get_random_float();
        }
    }
}

int emod(int a, int mod){
  return (a%mod + mod) % mod;
}


float sig1(float x, float a)
{
    return 1.0f/(1.0f + expf(-(x - a)*4/alpha_n));
}

float sig2(float x, float a, float b){
    return sig1(x, a)*(1 - sig1(x, b));
}


float sig_m(float x, float y, float m){
    return x*(1 - sig1(m, 0.5f)) + y*sig1(m, 0.5f);
}

float s(float n, float m){
    return sig2(n, sig_m(b_1, d_1, m), sig_m(b_2, d_2, m));
}


void compute_diff(){
  for(int y = 0; y < HEIGHT; y++){
    for(int x = 0; x < WIDTH; x++){
      float n = 0, m = 0, N = 0, M = 0;
      float r_i = r_a / 3.0;
  
     for(int i = -(r_a - 1); i < r_a; i++){
        for(int j = -(r_a - 1); j < r_a; j++){
          int cur_y = emod(y + j, HEIGHT);
          int cur_x = emod(x + i, WIDTH);

          if(i*i + j*j <= r_i*r_i){
            m += grid[cur_y][cur_x];
            M += 1;
          }else if(i*i + j*j <= r_a*r_a){
            n += grid[cur_y][cur_x];
            N += 1;
          }
        }
      }
      n /= N;
      m /= M;

      float diff_temp = s(n, m);
      grid_diff[y][x] = 2 * diff_temp - 1;
    }
  }
}

void clamp(float *x, float l, float h){
    if (*x < l) *x = l;
    if (*x > h) *x = h;
}

void apply_diff(void){
    for (size_t y = 0; y < HEIGHT; ++y) {
        for (size_t x = 0; x < WIDTH; ++x) {
            grid[y][x] += dt*grid_diff[y][x];
            clamp(&grid[y][x], 0, 1);
        }
    }
}




int main(){
  srand(time(0));

  initialize_random();
  for(;;){
    compute_diff();
    apply_diff();
    display_grid(grid);
  }
  return 0;
}
