import cv2
import numpy as np


def optim_func(return_img=False, **kwargs):
    #     print('kwargs: ', kwargs)
    geant4_noise = kwargs["noise_img"].copy()
    geant4_noise = cv2.GaussianBlur(geant4_noise, (3, 3), kwargs["blur_sigma"])
    geant4_noise_wo_jpeg = normalize_image(
        geant4_noise, min_val=0, max_val=kwargs["max_val"]
    )
    geant4_noise = jpeg_block_noise(
        geant4_noise_wo_jpeg, quality=kwargs["jpeg_quality"]
    )

    # calc histogram and compare
    if geant4_noise.dtype != np.uint8:
        geant4_noise = (255 * geant4_noise).astype(np.uint8)
    hist_noise = cv2.calcHist([geant4_noise], [0], None, [32], [0, 256]).flatten()
    gt_noise = kwargs["gt_noise"].copy()
    hist_gt = cv2.calcHist([gt_noise], [0], None, [32], [0, 256]).flatten()

    dist = calcEMD(hist_noise, hist_gt)
    #     dist = calcCORREL(hist_noise, hist_gt)

    if return_img:
        return dist, geant4_noise, gt_noise
    else:
        return dist


def convert_func(geant4_deposit, wo_noise, **kwargs):
    #     print('kwargs: ', kwargs)
    geant4_noise = geant4_deposit.copy()
    geant4_noise = cv2.GaussianBlur(geant4_noise, (3, 3), kwargs["blur_sigma"])
    geant4_noise_wo_jpeg = normalize_image(
        geant4_noise, min_val=0, max_val=kwargs["max_val"]
    )
    geant4_noise = jpeg_block_noise(
        geant4_noise_wo_jpeg, quality=kwargs["jpeg_quality"]
    )

    if geant4_noise.dtype != np.uint8:
        geant4_noise = (255 * geant4_noise).astype(np.uint8)

    # absolute noise model
    noisy_img = wo_noise.copy()
    mask = geant4_noise > wo_noise
    noisy_img[mask] = geant4_noise[mask]

    return noisy_img


def jpeg_block_noise(img, quality=50):
    encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), quality]
    if img.dtype != np.uint8:
        img = (255 * img).astype(np.uint8)
    result, encimg = cv2.imencode(".jpg", img, encode_param)
    noisy_img = cv2.imdecode(encimg, 0) / 255
    return noisy_img


def normalize_image(img, min_val=0, max_val=10):
    if min_val > max_val:
        print("flip min and max value")
        min_val, max_val = max_val, min_val
    img = np.clip((img - min_val) / (max_val - min_val), 0, 1)
    return img


# from scipy.stats import wasserstein_distance
def compareHist(hist1, hist2):
    hist1 = hist1.astype(np.float32)
    hist2 = hist2.astype(np.float32)
    ret = cv2.compareHist(hist1, hist2, cv2.HISTCMP_CORREL)
    #     ret1 = wasserstein_distance(hist1, hist2)
    # EMD
    emd = calcEMD(hist1, hist2)
    return ret, emd


def calcEMD(hist1, hist2):
    hist1 = hist1.astype(np.float32)
    hist2 = hist2.astype(np.float32)
    loc = np.arange(len(hist1), dtype=np.float32)
    sig1 = np.vstack([hist1, loc]).T
    sig2 = np.vstack([hist2, loc]).T
    emd, lb, flow = cv2.EMD(sig1, sig2, cv2.DIST_L1)
    return emd


def calcCORREL(hist1, hist2):
    hist1 = hist1.astype(np.float32)
    hist2 = hist2.astype(np.float32)
    ret = cv2.compareHist(hist1, hist2, cv2.HISTCMP_CORREL)
    return 1 - ret
