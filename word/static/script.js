async function loadDailyWords() {
  const res = await fetch('/get_daily');
  const data = await res.json();
  const dailyList = document.getElementById('daily-list');

  dailyList.innerHTML = data.words.map((word, index) => `
    <div class="word-card" data-index="${index}" onclick="openDetail(${index})">
      <h3>${word.head_word}</h3>
      <p>美音: /${word.usphone}/</p>
      <p>英音: /${word.ukphone}/</p>
      <p>${word.basic_meaning}</p>
    </div>
  `).join("");

  window.dailyWordsData = data.words;
}

function openDetail(index) {
  const word = window.dailyWordsData[index];
  const overlay = document.getElementById('detail-overlay');
  const detailBox = document.getElementById('word-detail');
  const card = document.querySelector(`.word-card[data-index="${index}"]`);
  const rect = card.getBoundingClientRect();

  detailBox.innerHTML = `
    <h2>${word.head_word}</h2>
    <p>美音: /${word.usphone}/</p>
    <p>英音: /${word.ukphone}/</p>
    <p>${word.basic_meaning}</p>
    <button onclick="closeDetail()">关闭</button>
  `;

  detailBox.style.transform = `translate(${rect.left}px, ${rect.top}px) scale(0.2)`;
  overlay.style.display = "flex";

  setTimeout(() => {
    detailBox.style.transform = "translate(0,0) scale(1)";
  }, 10);
}

function closeDetail() {
  const overlay = document.getElementById('detail-overlay');
  overlay.style.display = "none";
}

document.addEventListener("DOMContentLoaded", loadDailyWords);

async function loadFriends() {
  const res = await fetch('/get_friends');
  const data = await res.json();
  const friendsList = document.getElementById('friends-list');

  friendsList.innerHTML = data.map(f => `
    <div class="friend-card">
      <h3>${f.name} (${f.age}岁)</h3>
      <p>爱好：${f.hobbies.join("、")}</p>
      <button class="delete-btn" onclick="deleteFriend('${f.name}')">删除</button>
    </div>
  `).join("");
}

async function addFriend() {
  const name = document.getElementById('name').value.trim();
  const age = document.getElementById('age').value.trim();
  const hobbies = document.getElementById('hobbies').value.trim().split(',');

  if (!name || !age || hobbies.length === 0) return alert("请输入完整信息");

  await fetch('/add_friend', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ name, age: parseInt(age), hobbies })
  });

  loadFriends();
}

async function deleteFriend(name) {
  await fetch('/delete_friend', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ name })
  });

  loadFriends();
}

document.addEventListener("DOMContentLoaded", loadFriends);
