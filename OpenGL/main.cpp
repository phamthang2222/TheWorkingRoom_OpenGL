/*Chương trình chiếu sáng Blinn-Phong (Phong sua doi) cho hình lập phương đơn vị, điều khiển quay bằng phím x, y, z, X, Y, Z.*/

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

GLfloat l = -1.0, r = 1.0, t = 1.0, b = -1.0, zNear = 1, zFar = 6;
GLfloat dr = 5.0 * M_PI / 180;
GLfloat theta[] = { 0,0,0 };
GLfloat alpha[] = { 0,0,0 };
GLfloat alpha3 = 0, alpha4 = 0;
GLfloat quaycuatu[] = { 0,0,0 };
GLfloat quayquat = 0, quatcay = 0, Rkimphut = 0, Rkimgio = 0;


GLfloat zKeoTu = 0.0, zKeoGhe = 0.0;
GLfloat zKeongantu = 0.0;
GLfloat zKeomanchieu[] = {0,0,0,0,0,0,0,0,0,0 };

GLuint model_loc;
mat4 projection;
GLuint projection_loc;
mat4 view;
GLuint view_loc;
BOOLEAN quayquattran = false, quayquatcay = false, quaykimphut = true, quaykimgio = true;


GLfloat thetal = 0.0;
GLfloat xEye = 0.0, yEye = 0.0, zEye = 0.0; // tham sô tịnh tiến camera

mat4 model;
mat4 quayBase;

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 0.5, 0.0, 1.0); // orange
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);


}

/* Khởi tạo các tham số chiếu sáng - tô bóng*/
point4 light_position(-2.0, 2.0, 0.0, 0.0); // vị trí điểm sáng 
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0); //sáng chói

color4 material_ambient(1.0, 0.0, 1.0, 1.0);
color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
color4 material_specular(1.0, 0.8, 0.0, 1.0);
float material_shininess = 100.0;

color4 ambient_product = light_ambient * material_ambient;
color4 diffuse_product = light_diffuse * material_diffuse;
color4 specular_product = light_specular * material_specular;

void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}

vec4 mautuong = vec4(0.0, 1, 1, 0.5);
vec4 maubanghe = vec4(1.0, 0.0, 0.0, 1.0);
vec4 maubanghe2 = vec4(1.0, 0.6, 0.6, 1.0);
vec4 maukhungcuaso = vec4(1, 0.59, 0.12, 1.0);
vec4 maucanhcuaso = vec4(1, 0, 0.5, 0.6);
vec4 maungankeo = vec4(0.75, 1, 0.08, 0.6);
vec4 maumaytinh = vec4(0.0, 0, 0, 1.0);
vec4 maumaytinh2 = vec4(1, 1, 1, 1.0);
vec4 maumaytinh3 = vec4(0.5, 0.5, 0.5, 1.0);
vec4 mautu = vec4(0.8, 0.5, 0.5, 1.0);
vec4 mautham = vec4(0.0166, 0.0833, 1.0);
vec4 mauclock = vec4(1, 1, 0, 1.0);
vec4 maukim = vec4(0, 0, 0, 1);


void matPhang(GLfloat x, GLfloat y, GLfloat z, mat4 mt, vec4 color_of_obj) {

	material_diffuse = color_of_obj;
	diffuse_product = light_diffuse * material_diffuse;

	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	
	point4 eye(0+ xEye, 1 +yEye , 4+ zEye, 1.0);
	point4 at(-5* sin(thetal) + xEye, 1 + yEye, -5* cos(thetal) + zEye, 1.0);
	GLfloat dr = 5.0 * M_PI / 180;
	vec4 up(0, 1, 0, 1.0);

	mat4 v = LookAt(eye, at, up); // camera
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, v);

	mat4 ins = Scale(x, y, z);   // scale
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, quayBase * mt * ins);

	mat4 p = Frustum(l, r, b, t, zNear, zFar);  // view volumn
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, p);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// phòng làm việc 
void Wall_left() {
	model =  Translate(-3,1,0);
	matPhang(0.02, 2, 6, model, mautuong);
}
void Wall_Right() {
	model = Translate(3, 1, 0);
	matPhang(0.02, 2, 6, model, mautuong);
}
void Wall_Behind() {
	model = Translate(-2.2, 1, -3);
	matPhang(1.6, 2, 0.02, model, mautuong);

	model = Translate(2.2, 1, -3);
	matPhang(1.6, 2, 0.02, model, mautuong);

	model = Translate(0,1.75,-3 );
	matPhang(2.8, 0.5, 0.02, model, mautuong);

	model = Translate(0, 0.25, -3);
	matPhang(2.8, 0.5, 0.02, model, mautuong);

	model = Translate(0.5,1,-3);
	matPhang(0.2, 1, 0.04, model, mautuong);

	model = Translate(-0.5, 1, -3);
	matPhang(0.2, 1, 0.04, model, mautuong);
}
void Wall_inFontof(){
	model = Translate(-1.7 ,0.7 ,3) ;
	matPhang(2.6,1.4,0.02, model, mautuong);

	model = Translate(1.7, 0.7, 3);
	matPhang(2.6, 1.4, 0.02, model, mautuong);

	model = Translate(0, 1.7, 3);
	matPhang(6,0.6, 0.02, model, mautuong);

}


void San_Nha() {
	model = Translate(0, 0, 0) ;
	matPhang(6, 0.04, 6, model, mautuong); 
}
void Tran_Nha() {
	model = Translate(0, 2, 0);
	matPhang(6, 0.04, 6, model, mautuong);
}

void cuachinh() {
	model = Translate(-0.4, 0.7, 3) * RotateY(alpha4) * Translate(0.4, 0, 0);
	matPhang(0.8, 1.4, 0.02, model, maukhungcuaso);

	model = Translate(-0.4, 0.7, 3.03)*RotateY(alpha4) * Translate(0.7, 0, 0);
	matPhang(0.05, 0.05, 0.05, model, mautuong);
}

void khung_phong() { //sàn + 4 tường  + trần + cửa 
	quayBase = RotateY(theta[0]) * RotateX(theta[1])*Translate(0,0,-2) ;
	San_Nha();
	Tran_Nha();
	Wall_left();
	Wall_Right();
	Wall_Behind();
	Wall_inFontof();
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(0, 0, -2);
	cuachinh();
}



//bàn làm việc 
void chanban() {
	//chan 1
	model = Translate(-0.58, 0, -0.28);
	matPhang(0.04, 0.6, 0.04, model, maubanghe);
	//chân 2
	model = Translate(0.58, -0, -0.28);
	matPhang(0.04, 0.6, 0.04, model, maubanghe);
	//chân 3
	model = Translate(-0.58, -0, 0.28) ;
	matPhang(0.04, 0.6, 0.04, model, maubanghe);
	//chân 4
	model = Translate(0.58, -0, 0.28) ;
	matPhang(0.04, 0.6, 0.04, model, maubanghe);

}
void matban() {
	model = Translate(0, 0.3, 0);
	matPhang(1.2, 0.02, 0.6, model, maubanghe);
}
void ngankeo() {
	model = Translate(0, 0, zKeoTu) *  Translate(-0.36, 0.2, 0);
	matPhang(0.4, 0.2, 0.6, model, maungankeo);

	// tay nắm ngăn kéo 
	model = Translate(0, 0, zKeoTu) * Translate(-0.36, 0.24, 0.32);
	matPhang(0.04, 0.04, 0.04, model, mautuong);
	
}

void the_table() {
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(0, 0.3,-4);
	chanban();
	matban();
	ngankeo();
}

//cửa sổ 

void canh_cua1() {
	model = Translate(0, 0.5, 0)*RotateX(alpha[0])*Translate(0, -0.5, 0);
	matPhang(0.8, 1, 0.04, model, maucanhcuaso);
}
void canh_cua2() {
	model = Translate(-1.4, 0, 0)* RotateY(alpha[0]) * Translate(0.4, 0, 0);
	matPhang(0.8, 1, 0.04, model, maucanhcuaso);
}
void canh_cua3() {
	model = Translate(1.4, 0, 0) * RotateY(alpha[1]) * Translate(-0.4, 0, 0);
	matPhang(0.8, 1, 0.04, model, maucanhcuaso);
}
void windown() {
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(0, 1, -5);
	canh_cua1();
	canh_cua2();
	canh_cua3();
}

//cái ghế 
void mat_ghe() {
	model = Translate(0, 0, 0);
	matPhang(0.4, 0.02, 0.4, model, maubanghe2);
}
void chan_ghe() {
	//thanh dọc 
	model = Translate(0, -0.16, 0);
	matPhang(0.04, 0.3, 0.04, model, maubanghe2);
	// chân đế 
	model = RotateY(0) * Translate(0.09, -0.3, 0);
	matPhang(0.18, 0.02, 0.04, model, maubanghe2);

	model = RotateY(120) * Translate(0.09, -0.3, 0);
	matPhang(0.18, 0.02, 0.04, model, maubanghe2);

	model = RotateY(240) * Translate(0.09, -0.3, 0);
	matPhang(0.18, 0.02, 0.04, model, maubanghe2);
	
}
void tay_vin1() {
	//thanh doc
	model = Translate(0.19, 0.06, 0);
	matPhang(0.02, 0.12, 0.04, model, maubanghe2);
	//thanh ngang
	model = model * Translate(0, 0.08, 0);
	matPhang(0.05, 0.04, 0.2, model, maubanghe2);
}
void tay_vin2() {
	//thanh doc
	model = Translate(-0.19, 0.06, 0);
	matPhang(0.02, 0.12, 0.04, model, maubanghe2);
	//thanh ngang
	model = model * Translate(0, 0.08, 0);
	matPhang(0.05, 0.04, 0.2, model, maubanghe2);
}

void  tua_lung() {
	model = Translate(0, 0.2, 0.2);
	matPhang(0.4,0.4,0.02, model, maubanghe2);
}

void cai_ghe() {
	quayBase = RotateY(theta[0]) * RotateX(theta[1])*Translate(0.25,0.35, -3.5+ zKeoGhe);
	chan_ghe();
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(0.25, 0.35, -3.5 + zKeoGhe)*RotateY(alpha[3]);
	tay_vin1();
	tay_vin2();
	mat_ghe();
	tua_lung();
}


//máy tính 
void manhinh() {
	model = Translate(0,0,0);
	matPhang(0.5, 0.3, 0.02, model, maumaytinh2);

	model = Translate(0, 0, 0);
	matPhang(0.45, 0.25, 0.021, model, maumaytinh);

	model = Translate(0, -0.2, 0);
	matPhang(0.04, 0.1, 0.02, model, maumaytinh);

	model = Translate(0, -0.25, 0);
	matPhang(0.2, 0.02, 0.1, model, maumaytinh);
}
void ban_phim() {
	model = Translate(0, -0.25, 0.2);
	matPhang(0.3, 0.02, 0.1, model, maumaytinh);

	model = Translate(0, -0.25, 0.2);
	matPhang(0.28, 0.022, 0.08, model, maumaytinh2);
}
void maytinh() {
	quayBase = RotateY(theta[0]) * RotateX(theta[1])*Translate(-0.4,0.86,-4)*RotateY(30);
	manhinh();
	ban_phim();
}

//lap top 
void manhinh_lap() {
	//màn hình lap 
	model = Translate(0, -0.25, 0.2-0.15) * RotateX(alpha[2])*Translate(0, 0, 0.15);
	matPhang(0.4, 0.02, 0.3, model, maumaytinh);

	model = Translate(0, -0.25, 0.2-0.15) * RotateX(alpha[2])*Translate(0,0, 0.15);
	matPhang(0.38, 0.021, 0.28, model, maumaytinh2);
}
void ban_phim2() {

	//bàn phím lap 
	model = Translate(0, -0.25, 0.2);
	matPhang(0.4, 0.02, 0.3, model, maumaytinh);

	model = Translate(0, -0.25, 0.2);
	matPhang(0.38, 0.021, 0.28, model, maumaytinh3);
}

void laptop() {
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(0.3, 0.86, -4.15) ;
	
	ban_phim2();
	manhinh_lap();

}


// tủ đồ 
void than_tu() {
	// backgr
	model = Translate(0, 0, -0.2);
	matPhang(1.2, 1.4, 0.02, model, maukhungcuaso);

	//thanh tren
	model = Translate(0, 0.7, 0);
	matPhang(1.2, 0.02, 0.4, model, maukhungcuaso);

	//thanh duoi
	model = Translate(0, -0.7, 0);
	matPhang(1.2, 0.02, 0.4, model, maukhungcuaso);

	//thanh trai
	model = Translate(-0.6, 0, 0);
	matPhang(0.02, 1.4, 0.4, model, maukhungcuaso);

	//thanh phai
	model = Translate(0.6, 0, 0);
	matPhang(0.02, 1.4, 0.4, model, maukhungcuaso);

	//trục dọc  
	model = Translate(0, 0, 0);
	matPhang(0.02, 1.4, 0.4, model, maukhungcuaso);

	//trục ngang  
	model = Translate(0, 0, 0);
	matPhang(1.2, 0.02, 0.4, model, maukhungcuaso);

	//trục ngang  
	model = Translate(0, -0.35, 0);
	matPhang(1.2, 0.02, 0.4, model, maukhungcuaso);

	//trục ngang  
	model = Translate(0, 0.35, 0);
	matPhang(1.2, 0.02, 0.4, model, maukhungcuaso);
}
void tu_do() {
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(0, 0.72, -2)*Translate(2.78,0, -2)* RotateY(-90);
	than_tu();
}

// tu co ngan keo
void than_tu2() {
	model = Translate(0, 0, -0.2);
	matPhang(1.2, 1.4, 0.02, model, maukhungcuaso);

	//thanh tren
	model = Translate(0, 0.7, 0);
	matPhang(1.2, 0.02, 0.4, model, maukhungcuaso);

	//thanh duoi
	model = Translate(0, -0.7, 0);
	matPhang(1.2, 0.02, 0.4, model, maukhungcuaso);

	//thanh trai
	model = Translate(-0.6, 0, 0);
	matPhang(0.02, 1.4, 0.4, model, maukhungcuaso);

	//thanh phai
	model = Translate(0.6, 0, 0);
	matPhang(0.02, 1.4, 0.4, model, maukhungcuaso);

	//trục dọc  
	model = Translate(0, 0, 0);
	matPhang(0.02, 1.4, 0.4, model, maukhungcuaso);

	//trục ngang  
	model = Translate(0, -0.3, 0);
	matPhang(1.2, 0.02, 0.4, model, maukhungcuaso);
}
void canhtu_trai_tren() {
	model = Translate(-0.6, 0.2, 0.2) * RotateY(quaycuatu[0]) * Translate(0.3, 0, 0);
	matPhang(0.6, 1  , 0.02, model, mautu);

	model = Translate(-0.6, 0.2, 0.26) * RotateY(quaycuatu[0]) * Translate(0.5, 0, 0);
	matPhang(0.06, 0.04, 0.02, model, mautuong);
}
void canhtu_phai_tren() {
	model = Translate(+0.6, 0.2, 0.2) * RotateY(quaycuatu[1]) * Translate(-0.3, 0, 0);
	matPhang(0.6, 1, 0.02, model, mautu);

	model = Translate(0.6, 0.2, 0.26) * RotateY(quaycuatu[1]) * Translate(-0.5, 0, 0);
	matPhang(0.06, 0.04, 0.02, model, mautuong);
}
void ngantu_trai_duoi() {
	model = Translate(-0.3, -0.5, zKeongantu);
	matPhang(0.6, 0.38, 0.4, model, mautu);

	model = Translate(-0.3, -0.5, 0.22 + zKeongantu);
	matPhang(0.06, 0.04, 0.02, model, mautuong);
}
void ngantu_phai_duoi() {
	model = Translate(0.3, -0.5, zKeongantu);
	matPhang(0.6, 0.38, 0.4, model, mautu);

	model = Translate(0.3, -0.5, 0.22+ zKeongantu);
	matPhang(0.06, 0.04, 0.02, model, mautuong);
}
void tu_do2() {
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(0, 0.72, -2) * Translate(2.78, 0, -0.5) * RotateY(-90);
	than_tu2();
	canhtu_trai_tren();
	canhtu_phai_tren();
	
	ngantu_trai_duoi();
	ngantu_phai_duoi();
}

//quạt trần 
void thanquat() {
	model = Translate(0, 0, 0);
	matPhang(0.08, 0.12, 0.08, model, maungankeo);

	model = Translate(0, -0.1, 0);
	matPhang(0.03, 0.2, 0.03, model, maubanghe);

	model = Translate(0, -0.2, 0);
	matPhang(0.12, 0.02, 0.12, model, maungankeo);

	model = Translate(0, -0.25, 0);
	matPhang(0.12, 0.02, 0.12, model, maungankeo);
}
void canhquat() {
	model = Translate(0, 0, 0);
	matPhang(0.1, 0.4, 0.02, model, maubanghe);

	model = Translate(-0.02, 0.21, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);

	model = Translate(0, 0.21, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);
	
	model = Translate(0.02, 0.21, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);

	model = Translate(0.04, 0.21, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);

	model = Translate(0, 0.23, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);

	model = Translate(0.02, 0.23, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);

	model = Translate(0.04, 0.23, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);

	model = Translate(0.02, 0.25, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);

	model = Translate(0.04, 0.25, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);
}

void chumcanh() {
	//liên kêt tạo chụm cánh 

	quayBase = RotateY(theta[0]) * RotateX(theta[1])*Translate(0,1.77,-2) * RotateY(0 + quayquat) * RotateX(90) * Translate(0, 0.2, 0);
	canhquat();
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(0, 1.77, -2) *RotateY(120+ quayquat) *RotateX(90)* Translate(0, 0.2, 0);
	canhquat();
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(0, 1.77, -2) *RotateY(240+ quayquat) *RotateX(90)* Translate(0, 0.2, 0);
	canhquat();
		if (quayquattran == true) {
			quayquat -= 5;
			if (quayquat  < -360) quayquat -= 360;
			glutPostRedisplay();
		}	
}

void caiquat() {
	
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(0, 2, -2);
	thanquat();
	chumcanh();

}


//quạt cây 

void chande() {
	model = Translate(0, -0.21, 0);
	matPhang(0.15, 0.02, 0.15, model, maubanghe);
}
void thanquatcay() {
	model = Translate(0, 0, 0);
	matPhang(0.06, 0.4, 0.06, model, maumaytinh3);
}
void dauquat() {
	model = Translate(0, 0.24, 0);
	matPhang(0.2, 0.08, 0.08, model, maumaytinh2);
}
void canhquatcay() {
	model = Translate(0, 0.1, 0);
	matPhang(0.1, 0.2, 0.02, model, maubanghe);

	model = Translate(-0.02, 0.21, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);

	model = Translate(0, 0.21, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);

	model = Translate(0.02, 0.21, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);

	model = Translate(0.04, 0.21, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);

	model = Translate(0, 0.23, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);

	model = Translate(0.02, 0.23, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);

	model = Translate(0.04, 0.23, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);

	model = Translate(0.02, 0.25, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);

	model = Translate(0.04, 0.25, 0);
	matPhang(0.02, 0.02, 0.02, model, maubanghe);
}
void chumcanh2() {
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(-1.5, 0, -2) * Translate(0, 0.48, -1.9) * RotateZ(quatcay) * RotateY(90) * RotateX(0);
	canhquatcay();
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(-1.5, 0, -2) * Translate(0, 0.48, -1.9) * RotateZ(quatcay) * RotateY(90) * RotateX(120) ;
	canhquatcay();
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(-1.5, 0, -2) * Translate(0, 0.48, -1.9) * RotateZ(quatcay) * RotateY(90) * RotateX(240) ;
	canhquatcay();
	if (quayquatcay == true) {
		quatcay -= 2;
		if (quayquat < -360) quatcay -= 360;
		glutPostRedisplay();
	}
}
void quatcay1() {
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(-1.5, 0, -2) * Translate(0, 0.24, -2) * RotateY(-90);
	chande();
	thanquatcay();
	dauquat();
	chumcanh2();

}

//thảm nhà 
void tham() {
	model = RotateY(45);
	matPhang(2.5,0.03,2.5, model, mautham);

	model = RotateY(90);
	matPhang(1.76, 0.031, 1.76, model, maukhungcuaso);
}
void tham_san() {
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(0, 0.02, -2);
	tham();
}

// máy chiếu 
void thanmay() {
	model = Translate(0, 1.5, 0) ;
	matPhang(0.1,0.1,1, model,maumaytinh);
}
void manchieu() {
	model = Translate(0, 1.42, 0)  ;
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0,zKeomanchieu[0], 0) ;
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[1], 0) ;
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[2], 0) ;
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[3], 0) ;
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[4], 0) ;
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[5], 0) ;
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[6], 0) ;
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[7], 0) ;
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[8], 0) ;
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[9], 0);
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);
}
void maychieu1() {
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(-2.9,0,-2);
	thanmay();
	manchieu();
}

//máy chiếu 2
void thanmay2() {
	model = Translate(0, 1.5, 0);
	matPhang(0.1, 0.1, 1, model, maumaytinh);
}
void manchieu2() {
	model = Translate(0, 1.42, 0);
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[0], 0);
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[1], 0);
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[2], 0);
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[3], 0);
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[4], 0);
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[5], 0);
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[6], 0);
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[7], 0);
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[8], 0);
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);

	model = Translate(0, 1.42, 0) * Translate(0, zKeomanchieu[9], 0);
	matPhang(0.01, 0.08, 0.8, model, maumaytinh2);
}
void maychieu2() {
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(-2.9, 0, -0.8);
	thanmay();
	manchieu();
}

//đồng hồ 
void matdongho() {
	model = Translate(0,0,0);
	matPhang(0.6, 0.6, 0.01, model, mauclock);

	for (int i = 0; i < 12; i++) {

		model = RotateZ(30*i) *Translate(0, 0.28, 0.01);
		matPhang(0.02, 0.02, 0.01, model, maukim);
	}
	
}
void kim() {
	//kim phut
	model = RotateZ(Rkimphut)*Translate(0, 0.1, 0.02);
	matPhang(0.015, 0.2, 0.015, model, maukim);
	//kim gio
	model = RotateZ(Rkimgio) * Translate(0, 0.075, 0.02);
	matPhang(0.025, 0.15, 0.025, model, maukim);

	if (quaykimphut == true && quaykimgio == true) {
		Rkimphut -= 0.4;
		if (Rkimphut < -360) Rkimphut -= 360;

		Rkimgio -= (0.4/12);
		if (Rkimgio < -360) Rkimgio -= 360;

		glutPostRedisplay();
	}
}
void dongho() {
	quayBase = RotateY(theta[0]) * RotateX(theta[1]) * Translate(-2.98, 1.4, -4) *RotateY(90);
	matdongho();
	kim();
		
}

//--------------------------------------------------------
//tổng  hợp các thiết bị trong phòng 
void the_room() {
	
	khung_phong();

	the_table();

	windown();

	cai_ghe();

	maytinh();

	laptop();

	tu_do();
	
	tu_do2();

	caiquat();

	quatcay1();

	tham_san();

	maychieu1();

	maychieu2();

	dongho();
}
//--------------------------------------------------------

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	the_room();
	glutSwapBuffers();
}



void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
		//điều khiển camera
	case 'd':
		//theta[0] += 5;
		thetal -= 0.1;
		glutPostRedisplay();
		break;
	case 'a':
		//theta[0] -= 5;
		thetal += 0.1;
		glutPostRedisplay();
		break;
	case 'w': 
		zEye -= 0.1;
		break;
	case 's': 
		zEye += 0.1;
		break;

	case 'q':
		theta[1] += 5;
		glutPostRedisplay();
		break;
	case 'Q':
		theta[1] -= 5;
		glutPostRedisplay();
		break;
		//tăng  giảm view volumn
	case 'W':
		zNear *= 1.05; zFar *= 1.05;
		
		break;
	case 'S':
		zNear *= 0.95; zFar *= 0.95; 
	
		break;
	case 't': thetal += dr; break;
	case 'T': thetal -= dr; break;
	case '-':
		alpha4 += 5;
		if (alpha4 > 90) alpha4 = 90;

		glutPostRedisplay();
		break;
	case '_':
		alpha4 -= 5;
		if (alpha4 <- 90) alpha4 = -90;

		glutPostRedisplay();
		break;
	case 'o': // mở cửa sổ 
		alpha[0] += 5;
		alpha[1] -= 5;
		if (alpha[0] > 75) alpha[0] = 75;
		if (alpha[1] <-75) alpha[1] = -75;

		glutPostRedisplay();
		break;
	case 'O': // đóng cửa sổ 
		alpha[0] -= 5;
		alpha[1] += 5;
		if (alpha[0] < 0) alpha[0] = 0;
		if (alpha[1] >0) alpha[1] = 0;
		glutPostRedisplay();
		break;
	case 'k': // mở ngăn kéo 
		zKeoTu += 0.1;
		if (zKeoTu > 0.28) zKeoTu = 0.28;
		if (zKeoTu < 0) zKeoTu = 0;
		glutPostRedisplay();
		break;
	case 'K': // đóng  ngăn kéo 
		zKeoTu -= 0.1;
	
		if (zKeoTu < 0) zKeoTu = 0;
		glutPostRedisplay();
		break;

	case 'l': // keo ghee
		zKeoGhe += 0.1;

		if (zKeoGhe < 0) zKeoGhe = 0;
		glutPostRedisplay();
		break;
	case 'L': // keo ghee
		zKeoGhe -= 0.1;

		if (zKeoGhe < -0.1) zKeoGhe = -0.1;
		glutPostRedisplay();
		break;
	case '0':    //xoay ghế  
		alpha[3] += 5;
		glutPostRedisplay();
		break;
	case 'p':    // mở lap 
		alpha[2] -= 5;
		if (alpha[2] <- 120) alpha[2] = -120;
		glutPostRedisplay();
		break;
	case 'P':    // mở lap 
		alpha[2] += 5;
		if (alpha[2] > 0) alpha[2] = 0;
		glutPostRedisplay();
		break;
	case 'u':    // mở cánh cửa tủ  
		quaycuatu[0] -= 5;
		if (quaycuatu[0] < -120 ) quaycuatu[0] = -120;
		quaycuatu[1] += 5;
		if (quaycuatu[1] >120) quaycuatu[1] = 120;
		glutPostRedisplay();
		break;
	case 'U':    // mở cánh cửa tủ  
		quaycuatu[0] += 5;
		if (quaycuatu[0] >0) quaycuatu[0] = 0;
		quaycuatu[1] -= 5;
		if (quaycuatu[1] <0) quaycuatu[1] = 0;
		glutPostRedisplay();
		break;
	case 'j': // keo ngăn tủ 
		zKeongantu += 0.1;
		if (zKeongantu > 0.4) zKeongantu = 0.4;
		glutPostRedisplay();
		break;
	case 'J': // keo ngăn tủ 
		zKeongantu -= 0.1;
		if (zKeongantu <0 )zKeongantu = 0;
		glutPostRedisplay();
		break;
		
	case '1': //bật  quạt trần

		quayquattran = !quayquattran;
		
		glutPostRedisplay();
		break;
	case '2': //bật  quạt cây

		quayquatcay = !quayquatcay;

		glutPostRedisplay();
		break;
	case '3': //kéo màn chiếu 
			for (int i = 0; i < 10; i++) {
				zKeomanchieu[i] -= i * 0.08;
			}
		glutPostRedisplay();
		break;
	case '#': //thu màn chiếu  (shift 3)
			for (int i = 0; i < 10; i++) {
				zKeomanchieu[i] += i * 0.08;
			}
		glutPostRedisplay();
		break;
	
	}
	
	glutPostRedisplay();	
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Working Room");


	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	
	glutMainLoop();
	return 0;
}
