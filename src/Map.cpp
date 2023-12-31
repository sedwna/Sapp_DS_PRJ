#include "../include/Map.hpp"
const int INF = numeric_limits<int>::max();
// ------------------------------------------------------------------------------------------------------
void Map::createMap()
{

    bool greaterThan2 = false;
    bool greaterThan4 = false;
    int bg_count = 0;
    create_local_matrix();

    while (!greaterThan2)
    {
        cout << " number of province  " << endl;
        cout << "!!!your number should be greater than 2 -->  ";
        cin >> number_province;
        if (number_province > 2)
        {
            greaterThan2 = true;
        }
    }

    for (int i_p = 0; i_p < number_province; i_p++)
    {

        while (!greaterThan4)
        {
            cout << "number of city in province " << i_p << endl;
            cout << "!!!your number should be greater than 4 -->  ";
            cin >> number_city[i_p];
            cout << endl
                 << "enter  " << number_city[i_p] << "  cities -->";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (number_city[i_p] > 4)
            {
                greaterThan4 = true;
            }
        }

        for (int i_c = 0; i_c < number_city[i_p]; i_c++)
        {
            bool check = true;
            string line;
            string city_name;
            getline(cin, line);

            auto instruction = parse_CreateMap_instruction(line);
            city_name = get<0>(instruction);

            while (check)
            {
                for (int j = 0; j < number_city[i_p]; j++)
                {
                    if (city_name == province[i_p].city[j].city_name)
                    {
                        cout << "this name has already been selected." << endl;
                        cout << "choose another name ---> ";
                        cin >> city_name;
                        j = -1;
                    }
                }
                check = false;
            }

            province[i_p].city[i_c].city_name = city_name;
            province[i_p].city[i_c].id = get<1>(instruction);
            province[i_p].city[i_c].type = get<2>(instruction);
            province[i_p].city[i_c].province_name = get<3>(instruction);

            if (get<2>(instruction) == "BG")
            {
                set_bg_count();
            }
            set_node_id_local_matrix(i_c, i_p);
        }
        greaterThan4 = false;
    }
}
// ------------------------------------------------------------------------------------------------------
int **Map::createRoad()
{

    string line;
    int city_id_start;
    int city_id_end;
    int **foreign_matrix;

    foreign_matrix = create_foreign_matrix();

    cout << "enter cost of your road --> ";
    getline(cin, line);
    while (line != "-1")
    {

        auto instruction = parse_CreateRoad_instruction(line);
        city_id_start = found_city_id(get<1>(instruction), get<0>(instruction));
        city_id_end = found_city_id(get<4>(instruction), get<3>(instruction));

        if (get<1>(instruction) != get<4>(instruction)) // set bg node cost in foreign matrix
        {
            set_cost_foreign_matrix(instruction, foreign_matrix, city_id_start, city_id_end);
        }
        else if (get<1>(instruction) == get<4>(instruction)) // set node cost in local matrix
        {
            set_cost_local_matrix(instruction, foreign_matrix, city_id_start, city_id_end);
        }
        cout << "enter cost of your road --> ";
        getline(cin, line);
    }
    // show_local_matrix();
    return foreign_matrix;
}
// ------------------------------------------------------------------------------------------------------
tuple<string, string, int, string, string> Map::parse_CreateRoad_instruction(string road)
{

    string start_city;
    string end_city;
    string start_province;
    string end_province;
    int cost;

    stringstream check1(road);
    string intermediate;

    // CREATE (A.Qom) – [:ROAD {cost:50}] –> (K.Tabriz)

    // Tokenizing input
    getline(check1, intermediate, '('); // delete useless info
    getline(check1, intermediate, '.'); // get start_city name
    start_city = intermediate;          // push start_city name
    getline(check1, intermediate, ')'); // get start_province name
    start_province = intermediate;      // push start_province name
    getline(check1, intermediate, ':'); // delete useless info
    getline(check1, intermediate, ':'); // delete useless info
    getline(check1, intermediate, '}'); // get cost
    cost = stoi(intermediate);          // push cost
    getline(check1, intermediate, '('); // delete useless info
    getline(check1, intermediate, '.'); // get end_city name
    end_city = intermediate;            // push end_city name
    getline(check1, intermediate, ')'); // get end_province name
    end_province = intermediate;        // push end_province name
    return make_tuple(start_city, start_province, cost, end_city, end_province);
}
// ------------------------------------------------------------------------------------------------------
void Map::find_route_taken(int **foreign_matrix)
{
    cout << "Enter the find path command --> ";
    string line;
    getline(cin, line);

    auto instruction = parse_find_local(line);

    int start_id = found_city_id(get<0>(instruction), get<1>(instruction));
    int end_id = found_city_id(get<2>(instruction), get<3>(instruction));

    if (get<0>(instruction) == get<2>(instruction))
    {
        for (size_t i = 0; i < number_province; i++)
        {
            if (province[i].city[0].province_name == get<0>(instruction))
            {
                int k = 0;
                int sum = 0;
                while (province[i].city[k].id == end_id && province[i].local_matrix[start_id][end_id] == sum)
                {
                    for (size_t j = 0; j < number_city[i]; j++)
                    {
                        if (province[i].city[j].id == start_id)
                        {
                            for (k = 0; k < number_city[i]; k++)
                            {
                                if (province[i].copy_of_local_matrix[j][k] != 0)
                                {
                                    queue.enqueue(province[i].city[k].id);
                                    sum += province[i].copy_of_local_matrix[j][k];
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    else
    {
        int total_cost = 0;
        int temp1;
        int temp2;
        int bg_node_start;
        int bg_node_end;

        for (size_t k = 0; k < number_province; k++)
        {
            if (province[k].city[0].province_name == get<0>(instruction))
            {
                for (size_t j = 1; j < number_city[k] + 1; j++)
                {
                    for (size_t i = 1; i < bg_count + 1; i++)
                    {
                        if (foreign_matrix[i][0] == province[k].city[j].id)
                        {
                            temp1 = i;
                            bg_node_start = province[k].city[j].id;
                        }
                    }
                }
            }

            else if (province[k].city[0].province_name == get<2>(instruction))
            {
                for (size_t j = 1; j < number_city[k] + 1; j++)
                {
                    for (size_t i = 1; i < bg_count + 1; i++)
                    {
                        if (foreign_matrix[0][i] == province[k].city[j].id)
                        {
                            temp2 = i;
                            bg_node_end = province[k].city[j].id;
                        }
                    }
                }
            }
        }

        total_cost += foreign_matrix[temp1][temp2];

        for (size_t i = 0; i < number_province; i++)
        {
            if (province[i].city[0].province_name == get<0>(instruction))
            {
                for (size_t j = 1; j < number_city[i] + 1; j++)
                {
                    if (province[i].local_matrix[j][0] == bg_node_start)
                    {
                        for (size_t k = 1; k < number_city[i] + 1; k++)
                        {
                            if (province[i].local_matrix[0][k] == start_id)
                            {
                                total_cost += province[i].local_matrix[j][k];
                                break;
                            }
                        }
                    }
                }
            }
        }

        for (size_t i = 0; i < number_province; i++)
        {
            if (province[i].city[0].province_name == get<2>(instruction))
            {
                for (size_t j = 1; j < number_city[i] + 1; j++)
                {
                    if (province[i].local_matrix[j][0] == bg_node_end)
                    {
                        for (size_t k = 1; k < number_city[i] + 1; k++)
                        {
                            if (province[i].local_matrix[0][k] == end_id)
                            {
                                total_cost += province[i].local_matrix[j][k];
                                break;
                            }
                        }
                    }
                }
            }
        }

        cout << "Total Cost from " << get<0>(instruction) << "." << get<1>(instruction) << " to "
             << get<2>(instruction) << "." << get<3>(instruction) << " is : " << total_cost;
    }
}

// ------------------------------------------------------------------------------------------------------
tuple<string, string, string, string> Map::parse_find_local(string command)
{
    string start_city;
    string end_city;
    string start_province;
    string end_province;

    stringstream check1(command);
    string intermediate;

    // FIND AS4.W->AS1.F
    // Tokenizing find
    getline(check1, intermediate, ' ');  // delete useless info
    getline(check1, intermediate, '.');  // get start_province name
    start_province = intermediate;       // push start_province name
    getline(check1, intermediate, '-');  // get start_city name
    start_city = intermediate;           // push start_city name
    getline(check1, intermediate, '>');  // delete useless info
    getline(check1, intermediate, '.');  // get end_province name
    end_province = intermediate;         // push end_province name
    getline(check1, intermediate, '\0'); // get end_city name
    end_city = intermediate;             // push end_province name

    return make_tuple(start_province, start_city, end_province, end_city);
}

tuple<string, int, string, string> Map::parse_CreateMap_instruction(string create)
{

    string province_name;
    string city_name;
    string type;
    int id;
    stringstream check1(create);
    string intermediate;

    // Tokenizing input
    getline(check1, intermediate, '(');  // delete useless info
    getline(check1, intermediate, ':');  // get city name
    city_name = intermediate;            // push city name
    getline(check1, intermediate, '\''); // delete useless info
    getline(check1, intermediate, '\''); // get id
    id = stoi(intermediate);             // push id
    getline(check1, intermediate, '\''); // delete useless info
    getline(check1, intermediate, '\''); // get BG or  NBG
    type = intermediate;                 // //p[ush BG or NBG]
    getline(check1, intermediate, ' ');  // delete useless info
    getline(check1, intermediate, ':');  // get province name
    province_name = intermediate;        // get province name
    // cout << "complete parse" << endl;
    return make_tuple(city_name, id, type, province_name);
}
// ------------------------------------------------------------------------------------------------------
int Map::found_city_id(string Province_, string City_)
{

    for (size_t i = 0; i < number_province; i++)
    {
        for (size_t j = 0; j < number_city[i]; j++)
        {
            if (province[i].city[j].city_name == City_ && province[i].city[j].province_name == Province_)
            {
                return province[i].city[j].id;
            }
        }
    }
}
// ------------------------------------------------------------------------------------------------------
void Map::set_bg_count()
{
    bg_count++;
}
// ------------------------------------------------------------------------------------------------------
int Map::get_bg_count()
{
    return bg_count;
}
// ------------------------------------------------------------------------------------------------------
int **Map::create_foreign_matrix()
{

    int **foreign_matrix = new int *[bg_count + 1];
    for (int i = 0; i < bg_count + 1; ++i)
    {
        foreign_matrix[i] = new int[bg_count + 1];
        for (int j = 0; j < bg_count + 1; ++j)
        {
            foreign_matrix[i][j] = 0;
        }
    }

    int temp[bg_count];

    for (int i = 0; i < bg_count; i++)
    {
        temp[i] = -1;
    }

    for (int i = 0; i < number_province; i++)
    {

        for (int j = 0; j < number_city[i]; j++)
        {

            if (province[i].city[j].type == "BG")
            {

                for (int k = 0; k < bg_count; k++)
                {

                    if (temp[k] == -1)
                    {
                        temp[k] = province[i].city[j].id;
                        break;
                    }
                }
            }
        }
    }
    set_bg_id_foreign_matrix(foreign_matrix, temp);
    return foreign_matrix;
}
// ------------------------------------------------------------------------------------------------------
const void Map::show_foreign_matrix(int **foreign_matrix)
{
    cout << endl
         << "show foreign matrix" << endl;

    for (int i = 0; i < bg_count + 1; ++i)
    {
        for (int j = 0; j < bg_count + 1; ++j)
        {
            cout << left << setfill(' ') << setw(5) << foreign_matrix[i][j];
        }
        cout << endl;
    }
}
// ------------------------------------------------------------------------------------------------------
void Map::create_local_matrix() // set 0 in the local matrix
{

    for (size_t i = 0; i < number_province; i++)
    {
        for (size_t j = 0; j < SIZE_OF_CITY; j++)
        {
            for (size_t k = 0; k < SIZE_OF_CITY; k++)
            {
                province[i].set_local_matrix(j, k, 0);
            }
        }
    }
}
// ------------------------------------------------------------------------------------------------------
void Map::set_bg_id_foreign_matrix(int **foreign_matrix, int temp[]) // set id bg city in matrix
{

    for (int j = 0; j < bg_count; j++)
    {
        foreign_matrix[0][j + 1] = temp[j];
        foreign_matrix[j + 1][0] = temp[j];
    }
}
// ------------------------------------------------------------------------------------------------------
void Map::set_node_id_local_matrix(int i_c, int i_p) // set id in local matrix
{

    province[i_p].set_local_matrix(0, i_c + 1, province[i_p].city[i_c].id);
    province[i_p].set_local_matrix(i_c + 1, 0, province[i_p].city[i_c].id);
}
// ------------------------------------------------------------------------------------------------------
const void Map::show_local_matrix()
{
    for (size_t i = 0; i < number_province; i++)
    {
        cout << endl
             << " show local matrix "
             << endl;

        province[i].get_local_matrix();

        cout << endl;
    }
}
// ------------------------------------------------------------------------------------------------------
const void Map::set_cost_foreign_matrix(auto instruction, int **foreign_matrix, int city_id_start, int city_id_end)
{
    int temp1 = 0;
    int temp2 = 0;
    for (int j = 0; j < bg_count; j++)
    {
        if (foreign_matrix[0][j + 1] == city_id_start)
        {
            temp1 = j + 1;
            break;
        }
    }
    for (int j = 0; j < bg_count; j++)
    {
        if (foreign_matrix[j + 1][0] == city_id_end)
        {
            temp2 = j + 1;
            break;
        }
    }

    foreign_matrix[temp2][temp1] = get<2>(instruction);
    foreign_matrix[temp1][temp2] = get<2>(instruction);
}
// ------------------------------------------------------------------------------------------------------
const void Map::set_cost_local_matrix(auto instruction, int **foreign_matrix, int city_id_start, int city_id_end)
{
    for (size_t i = 0; i < number_province; i++)
    {

        if (province[i].city[0].province_name == get<1>(instruction))
        {
            int temp1 = 0;
            int temp2 = 0;
            for (int j = 0; j < SIZE_OF_CITY; j++)
            {
                if (province[i].local_matrix[0][j + 1] == city_id_start)
                {
                    temp1 = j + 1;
                    break;
                }
            }
            for (int j = 0; j < SIZE_OF_CITY; j++)
            {
                if (province[i].local_matrix[j + 1][0] == city_id_end)
                {
                    temp2 = j + 1;
                    break;
                }
            }
            province[i].local_matrix[temp2][temp1] = get<2>(instruction);
            province[i].local_matrix[temp1][temp2] = get<2>(instruction);
            province[i].copy_of_local_matrix[temp2][temp1] = get<2>(instruction);
        }
    }
}
// ------------------------------------------------------------------------------------------------------
const void Map::set_min_cost_local_matrix()
{
    for (int i = 0; i < number_province; i++) // send 'i' provinc to next loop
    {
        for (int j = 1; j <= number_city[i]; j++) // داخل این حلقه ماتریس داخلی استان مورد نظر و تعداد شهر ان به همراه سطر جی اُم  ماتریس رو میفرستیم به تابع دایجسترا تا کمترین کاست را درون ارایه دیستینسز بریزد
        {
            int distances[number_city[i]];
            gps.Dijkstra_local(province[i].local_matrix, number_city[i], j, distances);

            for (int k = 1; k <= number_city[i]; k++) // در این جا هم با ریختن مقادیر درون دیستینسز درون سطر متناظر با ان کاست ها را اپدیت می کنیم
            {
                if (distances[k] == INF)
                {
                    province[i].local_matrix[j][k] = 0;
                }
                else
                {
                    province[i].local_matrix[j][k] = distances[k];
                }
            }
        }
    }
}
// ------------------------------------------------------------------------------------------------------
const void Map::set_min_cost_foreign_matrix(int **foreign_matrix)
{

    for (int j = 1; j <= bg_count; j++)
    {
        int distances[bg_count];
        gps.Dijkstra_foreign(foreign_matrix, bg_count, j, distances);

        for (int k = 1; k <= bg_count; k++)
        {
            if (distances[k] == INF)
            {
                foreign_matrix[j][k] = 0;
            }
            else
            {
                foreign_matrix[j][k] = distances[k];
            }
        }
    }
}
// ------------------------------------------------------------------------------------------------------
