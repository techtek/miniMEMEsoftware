let refreshIntervalId;
let refreshCount = 0;
let countdownIntervalId;
let countdownTime = 0;
let countdownStart = 0;
let isRunning = false;
let notificationCount = 0;

// Show an alert when the background script is loaded
document.addEventListener('DOMContentLoaded', function() {
  var backgroundPage = chrome.extension.getBackgroundPage();
  if (backgroundPage) {
    backgroundPage.runBackgroundScript();
  }
});

function startRefresh() {
  const refreshTimeInput = document.getElementById('refreshTime');
  const refreshTime = parseInt(refreshTimeInput.value, 10);

  if (isNaN(refreshTime) || refreshTime < 1) {
    alert('Please enter a valid refresh time (in seconds).');
    return;
  }

  if (refreshTime < 10) {
    alert('The optimal refresh time is, and needs to be set above 10 seconds.');
    return;
  }

  if (!isRunning) {
    // Start button pressed for the first time
    clearInterval(refreshIntervalId);
    clearInterval(countdownIntervalId);
    refreshCount = 0;
    countdownTime = refreshTime;
    countdownStart = countdownTime;

    refreshIntervalId = setInterval(refreshPage, refreshTime * 1000);

    const counterElement = document.getElementById('refreshCount');
    counterElement.textContent = refreshCount;

    startCountdown();
    updateCountdown();

    const startButton = document.getElementById('refreshButton');
    startButton.textContent = 'Stop Refresh';
    startButton.classList.remove('start');
    startButton.classList.add('stop');

    const statusBar = document.getElementById('statusBar');
    statusBar.classList.remove('stop');
    statusBar.classList.add('start');

    isRunning = true;

    // Change the image source
    const headerImage = document.getElementById('headerImage');
    headerImage.src = 'Logo.png';
  } else {
    // Stop button pressed
    stopRefresh();
  }
}

function stopRefresh() {
  clearInterval(refreshIntervalId);
  clearInterval(countdownIntervalId);
  chrome.action.setBadgeText({ text: '' }); 
  console.log('Stop refresh button is pressed');  

  const startButton = document.getElementById('refreshButton');
  startButton.textContent = 'Start Refresh';
  startButton.classList.remove('stop');
  startButton.classList.add('start');

  const statusBar = document.getElementById('statusBar');
  statusBar.classList.remove('start');
  statusBar.classList.add('stop');
  
  isRunning = false;

  // Change the image source
  const headerImage = document.getElementById('headerImage');
  headerImage.src = 'Logooff.png';
}

async function refreshPage() {
  const clearCacheCheckbox = document.getElementById('clearCacheCheckbox');
  const shouldClearCache = clearCacheCheckbox.checked;

  if (shouldClearCache) {
    clearCache();
  }

  chrome.tabs.query({ active: true, currentWindow: true }, async function (tabs) {
    const tabId = tabs[0].id;

    await chrome.tabs.reload(tabId);

    chrome.tabs.onUpdated.addListener(function listener(updatedTabId, changeInfo) {
      if (updatedTabId === tabId && changeInfo.status === 'complete') {
        chrome.tabs.onUpdated.removeListener(listener);

        refreshCount++;
        const counterElement = document.getElementById('refreshCount');
        counterElement.textContent = refreshCount;

        // Sleep function for slow chromebooks 
        function sleep (seconds) {
          var start = new Date().getTime();
          while (new Date() < start + seconds*1000) {}
          return 0;
        }
        sleep(3);

        refreshImages(); // Fetch and display the new images after the page finishes loading
        
        resetCountdown();
        startCountdown();
      }
    });
  });
}

function clearCache() {
  chrome.browsingData.removeCache({}, function () {
    console.log('Cache cleared');
  });
}

function startCountdown() {
  clearInterval(countdownIntervalId);
  countdownIntervalId = setInterval(updateCountdown, 1000);
}

function resetCountdown() {
  clearInterval(countdownIntervalId);
  countdownTime = countdownStart;
  updateCountdown();
}

function updateCountdown() {
  const countdownElement = document.getElementById('countdown');
  const minutes = Math.floor(countdownTime / 60);
  const seconds = countdownTime % 60;
  countdownElement.textContent = `${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;

  countdownTime--;
  if (countdownTime < 0) {
    clearInterval(countdownIntervalId);
    countdownElement.textContent = '';
  }
}

document.addEventListener('DOMContentLoaded', function () {
  const popupContainer = document.documentElement; // Get the root element of the popup HTML
  updateCountdown();

  // Add click event listener to the root element
  popupContainer.addEventListener('click', function (event) {
    event.preventDefault();
  });

  const refreshButton = document.getElementById('refreshButton');
  const refreshTimeInput = document.getElementById('refreshTime');
  const testSoundElement = document.getElementById('testSound');
  testSoundElement.addEventListener('click', playNotificationSound);

  refreshButton.addEventListener('click', startRefresh);
  refreshTimeInput.addEventListener('keydown', function (event) {
    if (event.key === 'Enter') {
      refreshButton.click();
    }
  });

  const statusBar = document.getElementById('statusBar');
  statusBar.classList.add('stop');

  refreshImages();
  startRefresh();
  stopRefresh();   
  chrome.action.setBadgeText({ text: '' }); // Reset the badge counter to zero
  
  document.getElementById('refreshButton').addEventListener('click', refreshImages);

  // Test button for notification sound
  document.getElementById('testSoundButton').addEventListener('click', playNotificationSound);

  // Settings icon click event
  const settingsIcon = document.getElementById('settingsIcon');
  settingsIcon.addEventListener('click', openPopupPage2);
});

// splash screen display time
document.addEventListener('DOMContentLoaded', function () {
  var image = document.getElementById('myImage');
  setTimeout(function () {
    image.style.display = 'none';
  }, 900);
});

async function refreshImages() {
  const [tab] = await chrome.tabs.query({ active: true, currentWindow: true });
  const { url } = tab;

  let imageSrcs;
if (url.includes('sunpump.')) {
    const images = await chrome.scripting.executeScript({
      target: { tabId: tab.id },
      function: getSunPumpImagesCode,
    });
    imageSrcs = images[0].result;
  } 
  
  
  
  
  
  
  const getSunpumpImagesCode = () => {
  // Select images based on the specific class
  const images = Array.from(document.querySelectorAll('img[class*="_cover_"]')).map(img => img.src);
  return images;
};

  const filteredImages = imageSrcs.filter(isValidImage);
  const selectedImages = filteredImages.slice(0, 3); // Adjust as needed



  const container = document.getElementById('imageContainer');
  const existingImages = container.querySelectorAll('.small-image');
  const newImages = selectedImages.map(imageSrc => createImageElement(imageSrc));

  // Compare the fetched images with the newly fetched images
  if (existingImages.length === newImages.length) {
    const hasDifferentImages = Array.from(existingImages).some((existingImage, index) => {
      const newImage = newImages[index];
      return existingImage.src !== newImage.src;
    });

    if (hasDifferentImages) {
      playNotificationSound();
	    // Auto-save fetched images
        autoSaveImages(selectedImages);
    }
  }

  container.innerHTML = '';
  newImages.forEach(image => container.appendChild(image));
}

async function autoSaveImages(imageSrcs) {
  for (let i = 0; i < imageSrcs.length; i++) {
    const imageSrc = imageSrcs[i];
    try {
      const imageBlob = await fetchImageBlob(imageSrc);
      const imageUrl = URL.createObjectURL(imageBlob);

      // Create an image element to load the image
      const img = new Image();
      img.src = imageUrl;

      img.onload = function() {
        // Create a canvas to resize the image
        const canvas = document.createElement('canvas');
        const ctx = canvas.getContext('2d');

        // Define the maximum dimension
        const maxDim = 170;

        // Calculate the scaling factor
        const scale = Math.min(maxDim / img.width, maxDim / img.height);

        // Set the new canvas dimensions
        canvas.width = img.width * scale;
        canvas.height = img.height * scale;

        // Draw the resized image on the canvas
        ctx.drawImage(img, 0, 0, canvas.width, canvas.height);

        // Convert the canvas to a JPG blob
        canvas.toBlob(async (blob) => {
          const jpgUrl = URL.createObjectURL(blob);
          
          // Customize the filename for JPG
          const fileName = `image_${i + 1}.jpg`;

          chrome.downloads.download({
            url: jpgUrl,
            filename: fileName,
            saveAs: false  // Automatically saves without showing a dialog
          });

          // Clean up
          URL.revokeObjectURL(jpgUrl);
          URL.revokeObjectURL(imageUrl);
        }, 'image/jpeg');
      };
    } catch (error) {
      console.error('Failed to download image:', error);
    }
  }
}

// Helper function to fetch image as a Blob
async function fetchImageBlob(imageSrc) {
  const response = await fetch(imageSrc);
  if (!response.ok) {
    throw new Error('Network response was not ok.');
  }
  return await response.blob();
}

function createImageElement(imageSrc) {
  const img = document.createElement('img');
  img.src = imageSrc;
  img.className = 'small-image';
  img.addEventListener('click', function () {
    window.open(imageSrc, '_blank');
  });
  return img;
}

function isValidImage(imageSrc) {
  return imageSrc.startsWith('http') || imageSrc.startsWith('data');
}


function getSunPumpImagesCode() {
  return Array.from(document.querySelectorAll('img[class*="_cover_"]')).map(img => img.src);  
}



async function playNotificationSound() {
  const audio = new Audio();
  audio.src = 'notification.mp3';

  // Preload the audio file
  await audio.load();

  // Play the audio
  audio.play();

  // Increment the notification count
  notificationCount++;
  console.log('New items detected!');

  // Update the count display
  const countElement = document.getElementById('notificationCount');
  countElement.textContent = notificationCount.toString();

  // Apply the glowing effect to the image container
  const imageContainer = document.getElementById('imageContainer');
  imageContainer.classList.add('glow');

  // Remove the glowing effect after 7 seconds
  setTimeout(function() {
    imageContainer.classList.remove('glow');
  }, 7000);
}

function openPopupPage2() {
  chrome.runtime.openOptionsPage();
}

// Change extension icon
document.addEventListener('DOMContentLoaded', function() {
  var refreshButton = document.getElementById('refreshButton');

  refreshButton.addEventListener('click', function() {
    refreshButton.classList.toggle('start');
    refreshButton.classList.toggle('stop');

    // Change extension icon based on the status
    var iconPath = refreshButton.classList.contains('start') ? 'icon-start.png' : 'icon-stop.png';
    chrome.action.setIcon({ path: iconPath });
  });
});