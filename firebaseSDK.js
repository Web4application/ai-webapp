// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getAnalytics } from "firebase/analytics";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyC0ubx-bCPSEJxsQAM8-gOAZchaxquhRtI",
  authDomain: "web4-86e33.firebaseapp.com",
  projectId: "web4-86e33",
  storageBucket: "web4-86e33.firebasestorage.app",
  messagingSenderId: "641940543035",
  appId: "1:641940543035:web:6596dc15d2c2c913d1ee1e",
  measurementId: "G-S8TKPZXH47"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const analytics = getAnalytics(app);
