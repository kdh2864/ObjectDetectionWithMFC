import numpy as np

PATH_TO_CKPT = './models/frozen_inference_graph.pb'

class ObjectDetector(object):

  def __init__(self):
    import tensorflow as tf
    gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction = 0.4)
    self.tf = tf
    self.detection_graph = self._build_graph()
    self.sess = tf.Session(graph=self.detection_graph, config=tf.ConfigProto(gpu_options=gpu_options))

  def _build_graph(self):
    detection_graph = self.tf.Graph()
    with detection_graph.as_default():
      od_graph_def = self.tf.GraphDef()
      with self.tf.gfile.GFile(PATH_TO_CKPT, 'rb') as fid:
        serialized_graph = fid.read()
        od_graph_def.ParseFromString(serialized_graph)
        self.tf.import_graph_def(od_graph_def, name='')

    return detection_graph

  def detect(self, image_np):
    graph = self.detection_graph
    image_tensor = graph.get_tensor_by_name('image_tensor:0')
    boxes = graph.get_tensor_by_name('detection_boxes:0')
    scores = graph.get_tensor_by_name('detection_scores:0')
    classes = graph.get_tensor_by_name('detection_classes:0')
    num_detections = graph.get_tensor_by_name('num_detections:0')

    (boxes, scores, classes, num_detections) = self.sess.run(
        [boxes, scores, classes, num_detections],
        feed_dict={image_tensor: image_np})

    boxes, scores, classes, num_detections = map(
        np.squeeze, [boxes, scores, classes, num_detections])

    return boxes, scores, classes.astype(int), num_detections.astype(int)

client = ObjectDetector()