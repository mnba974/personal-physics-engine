#include <cmath>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm> 
#include <thread>
#include <future>

class vec2;
class pendulum;
class square;
class object;

bool cmp(object*, object*);




class vec2 {
public:
	double x, y;
	vec2() {

	}
	vec2(double x, double y) {
		this->x = x;
		this->y = y;
	}
	double getAngle() {
		return atan2(y, x);
	}
	vec2 operator+(vec2 obj) {
		return vec2(x + obj.x, y + obj.y);
	}
	vec2 operator+(double obj) {
		return vec2(x + obj, y + obj);
	}
	vec2 operator*(double obj) {
		return vec2(x * obj, y * obj);
	}
	vec2 operator-(vec2 obj) {
		return vec2(x - obj.x, y - obj.y);
	}
	vec2 operator-(double obj) {
		return vec2(x - obj, y - obj);
	}
	double norm() {
		return std::sqrt(x * x + y * y);
	}
};

class object {
public:
	vec2 position, prev_pos,initial_pos;
	vec2 velocity;
	double cap = 2;
	vec2 acc;
	double radius = 5;
	bool anchor = false;
	sf::Color color = sf::Color(151, 0, 0);

	object() {}
	object(vec2 pos) {
		position = pos;
		initial_pos = pos;
		prev_pos = pos;
		velocity = vec2(0,0);
		acc = vec2(0, 1000);
	}

	virtual void draw(sf::RenderWindow& window) {
		sf::CircleShape circle(radius);
		circle.setOrigin(radius, radius);
		if (anchor) {
			circle.setFillColor(sf::Color(0, 0, 150));
		}
		else {
			circle.setFillColor(color);
		}
		
		circle.setPosition(position.x, position.y);
		window.draw(circle);
	}

	virtual void update(double dt) {
		if (anchor == false) {
			velocity = position-prev_pos;
			if (velocity.norm() > cap) velocity = velocity * (cap / (velocity.norm()));
			prev_pos = position;
			position = position + velocity + acc * dt * dt;
			
		}
		else {
			position = initial_pos;
		}

	}
	virtual void constraint(object& obj) {}
	virtual std::vector<object*> getchildren() {
		std::vector<object*> objs;
		return objs;
	}
	virtual void collisionhelper(object* obj, double distance) {
		double len = (position - obj->position).norm();
		double diff = (len - distance) / 2;
		position = position + (obj->position - position) * ((diff) / len);
		obj->position = obj->position + (position - obj->position) * ((diff) / len);

	}
	
	virtual void collision(std::vector<object*> objs) {
		for (object* obj : objs) {
			if (obj == this) continue;
			double distance = radius + obj->radius;
			if ((position - obj->position).norm() < distance) {
				collisionhelper(obj, distance);
			}
			
			
		}

	}



	virtual void add(std::vector<object*>& objstotal){}
};
class linker : public object {
public:
	object* obj1;
	object* obj2;
	double linklen = 200;
	linker() {}
	linker(object* xobj1, object* xobj2, double linkl = 200) {
		obj1 = xobj1;
		obj2 = xobj2;
		linklen = linkl;
	}
	void update(double dt) override {
		double len = ((*obj1).position - (*obj2).position).norm();
		if ((*obj1).anchor == true) (*obj2).position = (*obj1).position + ((*obj2).position - (*obj1).position) * ((linklen) / len);
		else if ((*obj2).anchor == true)(*obj1).position = (*obj2).position + ((*obj1).position - (*obj2).position) * ((linklen) / len);
		else {
			double diff = (len - linklen) / 2;
			(*obj2).position = (*obj2).position + ((*obj1).position - (*obj2).position) * ((diff) / len);
			(*obj1).position = (*obj1).position + ((*obj2).position - (*obj1).position) * ((diff) / len);
		}
	}

	void draw(sf::RenderWindow& window) override {
		sf::Vertex line[] =
		{
			sf::Vertex(sf::Vector2f(obj1->position.x,obj1->position.y)),
			sf::Vertex(sf::Vector2f(obj2->position.x,obj2->position.y))
		};

		window.draw(line, 2, sf::Lines);
	}
	void collision(std::vector<object*> objstotal) override {}

};

class square : public object {
public:
	object* obj1, * obj2, * obj3, * obj4, *obj5, * obj6, * obj7, * obj8;
	linker* l1, * l2, * l3, * l4, * l5, *l6, * l7, * l8, * l9,*l10,*l11,*l12;
	std::vector<object*> objs;
	double size = 60;
	square(vec2 pos, int s = 30) {
		size = s;
		position = pos;
		obj1 = new object(pos - size);
		obj2 = new object(vec2(pos.x, pos.y - size));
		obj3 = new object(vec2(pos.x + size, pos.y - size));
		obj4 = new object(vec2(pos.x + size, pos.y));
		obj5 = new object(pos + size);
		obj6 = new object(vec2(pos.x, pos.y + size));
		obj7 = new object(vec2(pos.x - size, pos.y + size));
		obj8 = new object(vec2(pos.x - size, pos.y));
		
		l1 = new linker(obj1, obj2, size);
		l2 = new linker(obj2, obj3, size);
		l3 = new linker(obj3, obj4, size);
		l4 = new linker(obj4, obj5, size);
		l5 = new linker(obj5, obj6, size);
		l6 = new linker(obj6, obj7, size);
		l7 = new linker(obj7, obj8, size);
		l8 = new linker(obj8, obj1, size);
		l9 = new linker(obj2, obj6, 2*size);
		l10 = new linker(obj4, obj8, 2 * size);
		l11 = new linker(obj1, obj5, 2 *1.41* size);
		l12 = new linker(obj3, obj7, 2 *1.41* size);
		objs = {obj1,obj2,obj3,obj4,obj5,obj6,obj7,obj8, l1,l2,l3,l4,l5,l6,l7,l8,l9,l10,l11,l12 };
		for (int rescl = 0;rescl < 8;rescl++) {
			objs[rescl]->radius = size / 2;
		}

	}
	void update(double dt) override {
		for (int i = 0;i < objs.size();i++) {
			(*objs[i]).update(dt);
		}
	}
	void draw(sf::RenderWindow& window) override {
		for (int i = 0;i < objs.size();i++) {
			(*objs[i]).draw(window);
		}
	}
	std::vector<object*> getchildren() override {
		return objs;
	}
	void collision(std::vector<object*> objstotal) override {
		for (object* obj : objs) {
			obj->collision(objstotal);
		}
	}
	void add(std::vector<object*> &objstotal) override{
		for (object* obj : objs) {
			objstotal.emplace_back(obj);
		}
	}

};


class container : public object {
public:
	double radius;
	container() {
		position = vec2(650, 400);
		radius = 400;
	}
	void update(double dt) override {}

	void draw(sf::RenderWindow& window) override {
		sf::CircleShape circle(radius, 100);
		circle.setOrigin(radius, radius);
		circle.setFillColor(sf::Color(0, 0, 0));
		circle.setPosition(position.x, position.y);
		window.draw(circle);
	}

	void constraint(object& obj) override {
		if (typeid(obj) != typeid(object)) {
			std::vector<object*> objs = obj.getchildren();
			for (object* obj : objs) {
				constraint(*obj);
			}
		}
		else {
			double len = (position - obj.position).norm();
			if (len > radius - obj.radius) {
				obj.position = position + (obj.position - position) * ((radius - obj.radius) / len);

			}
		}

	}
};
class rectContainer : public object {
public:
	sf::RectangleShape rect;
	rectContainer() {
		rect.setSize(sf::Vector2f(1300, 800));
		rect.setFillColor(sf::Color(0,0,0));
	}
	void update(double dt) override {}

	void draw(sf::RenderWindow& window) override {
		window.draw(rect);
	}

	void constraint(object& obj) override {
		if (obj.position.x < obj.radius) obj.position.x = obj.radius;
		if (obj.position.x > 1300-obj.radius) obj.position.x = 1300 - obj.radius;
		if (obj.position.y < obj.radius) obj.position.y = obj.radius;
		if (obj.position.y > 800 - obj.radius) obj.position.y = 800 - obj.radius;
	}

};




class renderer {
public:
	std::vector<object*> objects;
	std::vector<object*> constraints;
	sf::RenderWindow* window;
	std::vector<std::vector<std::vector<object*>>> grid;
	int partition = 20;

	renderer() {
	}
	renderer(std::vector<object*>& obj, std::vector<object*>& consts, sf::RenderWindow* win) {
		objects = obj;
		constraints = consts;
		window = win;
		grid = std::vector<std::vector<std::vector<object*>>>(800/ partition + 10, std::vector<std::vector<object*>> (1300/ partition + 10, std::vector <object*>(0) ) );
	}
	
	void render() {
		for (int i = 0;i < constraints.size();i++) {
			(*constraints[i]).draw(*window);
		}

		for (int i = 0;i < objects.size();i++) {
			(*objects[i]).draw(*window);
			//std::async(std::launch::async,object::draw, objects[i], *window);
		}

	}
	void updateGrid() {
		for (auto& row : grid) {
			for (auto& cell : row) {
				cell.clear();
			}
		}
		for (object* obj : objects) {
			int x = static_cast<int>((obj->position.x) / partition) + 5;
			int y = static_cast<int>((obj->position.y) / partition) + 5;
			grid[y][x].emplace_back(obj);
		}
	}

	void processgrid(int start, int finish) {
		for (int i = start; i < finish; ++i) {
			for (int j = 1; j < grid[i].size() - 1; ++j) {
				for (object* obj : grid[i][j]) {
					obj->collision(grid[i - 1][j - 1]);
					obj->collision(grid[i - 1][j]);
					obj->collision(grid[i - 1][j + 1]);
					obj->collision(grid[i][j - 1]);
					obj->collision(grid[i][j]);
					
					obj->collision(grid[i][j + 1]);
					obj->collision(grid[i + 1][j - 1]);
					obj->collision(grid[i + 1][j]);
					obj->collision(grid[i + 1][j + 1]);
				}
			}
		}
	}
	void gridconstraint(object* constraint) {
		for (int i = 0;i < 2;i++) {
			for (int j = 0;j < grid[0].size();j++) {
				for (object* obj : grid[i][j]) {
					constraint->constraint(*obj);
				}
			}
		}
		for (int i = grid.size() - 2;i < grid.size();i++) {
			for (int j = 0;j < grid[0].size();j++) {
				for (object* obj : grid[i][j]) {
					constraint->constraint(*obj);
				}
			}
		}
		for (int j = 0;j < 2;j++) {
			for (int i = 0;i < grid.size();i++) {
				for (object* obj : grid[i][j]) {
					constraint->constraint(*obj);
				}
			}
		}
		for (int j = grid[0].size()-2;j < grid[0].size();j++) {
			for (int i = 0;i < grid.size();i++) {
				for (object* obj : grid[i][j]) {
					constraint->constraint(*obj);
				}
			}
		}

	}


	void update(double dt, int iters = 1) {
		dt = dt / iters;

		//std::sort(objects.begin(), objects.end(), cmp);
		for (int x = 0; x < iters;x++) {

			for (int i = 0;i < objects.size();i++) {
				(*objects[i]).update(dt);
				//std::async(std::launch::async ,&object::update, objects[i], dt);
			}
			
			/*
			for (int i = 0;i < objects.size();i++) {
				(*objects[i]).collision(std::vector<object*>(objects.begin() + i + 1, objects.end()));
				//std::async(std::launch::async ,&object::collision, objects[i], std::vector<object*>(objects.begin() + i + 1, objects.end()));
			}
			*/
			
			for (int i = 0;i < constraints.size();i++) {
				(*constraints[i]).update(dt);
				for (int j = 0;j < objects.size();j++) {
					(*constraints[i]).constraint((*objects[j]));
				}
			}
			updateGrid();
			/*
			
			for (int i = 0;i < constraints.size();i++) {
				(*constraints[i]).update(dt);
				gridconstraint(constraints[i]);
			}
			*/
			
			//processgrid(5, grid.size() - 5);
			/*
			std::thread t1(&renderer::processgrid, this, 1, grid.size() / 2);
			std::thread t3(&renderer::processgrid, this, grid.size() / 2, grid.size() - 1);
			t1.join();
			t3.join();
			*/
			int numThreads = std::thread::hardware_concurrency();
			std::vector<std::future<void>> futures;

			int step = grid.size() / numThreads;
			for (int i = 0; i < numThreads; i++) {
				int start = i * step;
				int end = (i == numThreads - 1) ? grid.size() : (i + 1) * step;
				futures.emplace_back(std::async(std::launch::async, &renderer::processgrid, this, start, end));
			}

			for (auto& future : futures) {
				future.get();
			}
			

			
		}


	}
};

class interaction {
public:
	bool lpressed = false;
	bool rpressed = false;
	object* linkerobj;
	
	int linkerprogression = 0;
	object* obj;
	interaction() {}

	object* getObject(renderer rend,vec2 pos) {
		object* returnobj = NULL;
		for (object* obj : rend.objects) {
			if ((obj->position - pos).norm() < obj->radius) {
				returnobj = obj;
				break;
			}

		}
		return returnobj;
	}

	void mainloop(renderer& rend,char selection) {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && lpressed == false) {
			sf::Vector2i position = sf::Mouse::getPosition(*(rend.window));
			vec2 pos(position.x, position.y);
			std::cout << pos.x << ' ' << pos.y << '\n';
			switch (selection) {
			case 'b':
				obj = new object(pos);
				rend.objects.emplace_back(obj);
				break;
			case 's':
				obj = new square(pos);
				obj->add(rend.objects);
				break;
			case 'l':
				obj = getObject(rend, pos);
				if (obj != NULL) {
					switch (linkerprogression) {
					case 0:
						linkerobj = obj;
						linkerprogression += 1;
						break;
					case 1:
						if (obj != linkerobj) {
							rend.objects.emplace_back(new linker(linkerobj, obj, (linkerobj->position - obj->position).norm()));
							linkerobj = obj;
						}
					}
				}

			}
			
			
			lpressed = true;
		}
		else if (!(sf::Mouse::isButtonPressed(sf::Mouse::Left))) {
			lpressed = false;
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && rpressed == false) {
			sf::Vector2i position = sf::Mouse::getPosition(*(rend.window));
			vec2 pos(position.x, position.y);
			obj = getObject(rend, pos);
			if (obj != NULL) {
				obj->anchor = !obj->anchor;
				obj->initial_pos = obj->position;
			}
			
			rpressed = true;
		}
		else if (!(sf::Mouse::isButtonPressed(sf::Mouse::Right))) {
			rpressed = false;
		}
	}
};