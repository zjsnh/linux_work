import json
import random
from flask import Flask, render_template, jsonify, request

app = Flask(__name__)

# 定义文件路径
FRIENDS_FILE = 'hunjie.json'

# 读取好友数据
def load_friends():
    try:
        with open(FRIENDS_FILE, 'r', encoding='utf-8') as file:
            return json.load(file)
    except (FileNotFoundError, json.JSONDecodeError):
        return []  # 如果没有文件或文件格式错误，返回空列表

# 保存好友数据
def save_friends(friends):
    with open(FRIENDS_FILE, 'w', encoding='utf-8') as file:
        json.dump(friends, file, ensure_ascii=False, indent=2)


# 渲染首页（介绍页）
@app.route('/')
def index():
    return render_template('index.html')

# 渲染单词搜索页面
@app.route('/search')
def search_page():
    return render_template('search.html')

# 渲染每日单词页面
@app.route('/daily')
def daily_page():
    return render_template('daily.html')

# 渲染好友页面
@app.route('/friend')
def friend_page():
    return render_template('friend.html')

@app.route('/random')
def random_page():
    return render_template('random.html')

# 获取好友数据（返回 JSON）
@app.route('/get_friends', methods=['GET'])
def get_friends():
    return jsonify(load_friends())

# 添加好友
@app.route('/add_friend', methods=['POST'])
def add_friend():
    data = request.json
    if not data or not all(k in data for k in ("name", "age", "hobbies")):
        return jsonify({"error": "缺少必要信息"}), 400

    friends = load_friends()
    friends.append(data)
    save_friends(friends)

    return jsonify({"message": "添加成功", "friend": data})

# 删除好友
@app.route('/delete_friend', methods=['POST'])
def delete_friend():
    data = request.json
    name_to_delete = data.get("name")

    friends = load_friends()
    friends = [f for f in friends if f["name"] != name_to_delete]
    save_friends(friends)

    return jsonify({"message": "删除成功"})

# 分页获取单词数据，每页20个（用于每日单词页面等）
@app.route('/get_words', methods=['GET'])
def get_words():
    page = int(request.args.get('page', 1))
    page_size = 20
    start = (page - 1) * page_size
    end = start + page_size

    file_path = 'KaoYan_2.json'
    words_data = []

    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            lines = file.readlines()
    except Exception as e:
        return jsonify({"error": f"无法读取文件: {e}"}), 500

    for line in lines[start:end]:
        try:
            entry = json.loads(line)
            head_word = entry.get('headWord')
            content = entry.get('content', {})
            word_data = content.get('word', {})
            word_content = word_data.get('content', {})
            usphone = word_content.get('usphone', 'N/A')
            ukphone = word_content.get('ukphone', 'N/A')
            translations = word_content.get('trans', [])
            # 取第一条翻译作为卡片基本释义
            basic_meaning = ""
            if translations and isinstance(translations, list):
                basic_meaning = translations[0].get('tranCn', "")
            # 例句处理
            sentences = word_content.get('sentence', {}).get('sentences', [])
            sentences_list = []
            for sen in sentences:
                sentences_list.append({
                    "content": sen.get('sContent', ''),
                    "cn": sen.get('sCn', '')
                })
            # 短语处理
            phrases = word_content.get('phrase', {}).get('phrases', [])
            phrases_list = []
            for ph in phrases:
                phrases_list.append({
                    "content": ph.get('pContent', ''),
                    "cn": ph.get('pCn', '')
                })

            word_info = {
                "head_word": head_word,
                "usphone": usphone,
                "ukphone": ukphone,
                "basic_meaning": basic_meaning,
                "translations": translations,  # 完整翻译信息
                "sentences": sentences_list,
                "phrases": phrases_list
            }
            words_data.append(word_info)
        except json.JSONDecodeError:
            continue

    total_lines = len(lines)
    total_pages = total_lines // page_size + (1 if total_lines % page_size != 0 else 0)
    return jsonify({
        "words": words_data,
        "total_pages": total_pages
    })

# 搜索接口：根据 query 返回第一个匹配到的完整单词信息
@app.route('/search_word', methods=['GET'])
def search_word():
    query = request.args.get('query', '').strip()
    file_path = 'KaoYan_2.json'
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            for line in file:
                try:
                    entry = json.loads(line)
                    head_word = entry.get('headWord', '')
                    if query.lower() in head_word.lower():
                        content = entry.get('content', {})
                        word_data = content.get('word', {})
                        word_content = word_data.get('content', {})
                        usphone = word_content.get('usphone', 'N/A')
                        ukphone = word_content.get('ukphone', 'N/A')
                        translations = word_content.get('trans', [])
                        sentences = word_content.get('sentence', {}).get('sentences', [])
                        sentences_list = []
                        for sen in sentences:
                            sentences_list.append({
                                "content": sen.get('sContent', ''),
                                "cn": sen.get('sCn', '')
                            })
                        phrases = word_content.get('phrase', {}).get('phrases', [])
                        phrases_list = []
                        for ph in phrases:
                            phrases_list.append({
                                "content": ph.get('pContent', ''),
                                "cn": ph.get('pCn', '')
                            })
                        word_info = {
                            "head_word": head_word,
                            "usphone": usphone,
                            "ukphone": ukphone,
                            "translations": translations,
                            "sentences": sentences_list,
                            "phrases": phrases_list
                        }
                        return jsonify({"result": word_info})
                except json.JSONDecodeError:
                    continue
        return jsonify({"result": None, "message": "未找到匹配单词"})
    except Exception as e:
        return jsonify({"error": str(e)}), 500

# 每日单词接口：随机返回一个包含20个单词的页面
@app.route('/get_daily', methods=['GET'])
def get_daily():
    file_path = 'KaoYan_2.json'
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            lines = file.readlines()
    except Exception as e:
        return jsonify({"error": f"无法读取文件: {e}"}), 500
    total_lines = len(lines)
    page_size = 20
    total_pages = total_lines // page_size + (1 if total_lines % page_size != 0 else 0)
    random_page = random.randint(1, total_pages)
    start = (random_page - 1) * page_size
    end = start + page_size
    words_data = []
    for line in lines[start:end]:
        try:
            entry = json.loads(line)
            head_word = entry.get('headWord')
            content = entry.get('content', {})
            word_data = content.get('word', {})
            word_content = word_data.get('content', {})
            usphone = word_content.get('usphone', 'N/A')
            ukphone = word_content.get('ukphone', 'N/A')
            translations = word_content.get('trans', [])
            basic_meaning = ""
            if translations and isinstance(translations, list):
                basic_meaning = translations[0].get('tranCn', "")
            sentences = word_content.get('sentence', {}).get('sentences', [])
            sentences_list = []
            for sen in sentences:
                sentences_list.append({
                    "content": sen.get('sContent', ''),
                    "cn": sen.get('sCn', '')
                })
            phrases = word_content.get('phrase', {}).get('phrases', [])
            phrases_list = []
            for ph in phrases:
                phrases_list.append({
                    "content": ph.get('pContent', ''),
                    "cn": ph.get('pCn', '')
                })
            word_info = {
                "head_word": head_word,
                "usphone": usphone,
                "ukphone": ukphone,
                "basic_meaning": basic_meaning,
                "translations": translations,
                "sentences": sentences_list,
                "phrases": phrases_list
            }
            words_data.append(word_info)
        except json.JSONDecodeError:
            continue
    return jsonify({
        "words": words_data,
        "page": random_page,
        "total_pages": total_pages
    })

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8888, debug=True)