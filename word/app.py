import json
from flask import Flask, render_template, jsonify

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/get_words', methods=['GET'])
def get_words():
    file_path = 'KaoYan_2.json'
    words_data = []
    count = 0

    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            lines = file.readlines()
    except Exception as e:
        return jsonify({"error": f"无法读取文件: {e}"}), 500

    for line in lines:
        if count >= 4000:
            break
        try:
            entry = json.loads(line)
            word_rank = entry.get('wordRank')
            head_word = entry.get('headWord')
            content = entry.get('content', {})
            word = content.get('word', {})
            word_head = word.get('wordHead')
            word_id = word.get('wordId')
            word_content = word.get('content', {})

            # 提取例句
            sentences = word_content.get('sentence', {}).get('sentences', [])
            sentences_list = [
                {"content": sentence.get('sContent'), "cn": sentence.get('sCn')}
                for sentence in sentences
            ]

            # 发音、短语、记忆方法、相关词、翻译等
            usphone = word_content.get('usphone')
            ukphone = word_content.get('ukphone')
            ukspeech = word_content.get('ukspeech')
            star = word_content.get('star')
            phrases = word_content.get('phrase', {}).get('phrases', [])
            phrases_list = [
                {"content": phrase.get('pContent'), "cn": phrase.get('pCn')}
                for phrase in phrases
            ]
            phone = word_content.get('phone')
            speech = word_content.get('speech')
            rem_method = word_content.get('remMethod', {})
            rel_words = word_content.get('relWord', {}).get('rels', [])
            rel_words_list = [
                {"pos": rel_word.get('pos'), "words": [w.get('hwd') for w in rel_word.get('words', [])]}
                for rel_word in rel_words
            ]
            usspeech = word_content.get('usspeech')
            translations = word_content.get('trans', [])
            translations_list = [
                {"tranCn": translation.get('tranCn'),
                 "descCn": translation.get('descCn'),
                 "pos": translation.get('pos')}
                for translation in translations
            ]

            # 构建单词信息字典
            word_info = {
                "word_rank": word_rank,
                "head_word": head_word,
                "word_head": word_head,
                "word_id": word_id,
                "sentences": sentences_list,
                "usphone": usphone,
                "ukphone": ukphone,
                "ukspeech": ukspeech,
                "star": star,
                "phrases": phrases_list,
                "phone": phone,
                "speech": speech,
                "rem_method": {"val": rem_method.get('val'), "desc": rem_method.get('desc')},
                "rel_words": rel_words_list,
                "usspeech": usspeech,
                "translations": translations_list
            }

            words_data.append(word_info)
            count += 1
        except json.JSONDecodeError:
            continue

    return jsonify(words_data)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8888, debug=True)