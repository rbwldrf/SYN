export var adapter;
export var device;
export var context;

export var ctTexture;
export var depthTexture;

export var canvas;

export async function init(){

	adapter = await navigator.gpu.requestAdapter();
	device = await adapter.requestDevice();

	canvas = document.querySelector("#syn");
	context = canvas.getContext("webgpu");

	context.configure({ 
		device: device, 
		format: navigator.gpu.getPreferredCanvasFormat(), 
		usage:  GPUTextureUsage.COPY_SRC |
			GPUTextureUsage.RENDER_ATTACHMENT
	});

	ctTexture = context.getCurrentTexture();
	depthTexture = device.createTexture({
		size: [ctTexture.width,ctTexture.height,1],
		usage:  GPUTextureUsage.COPY_SRC | 
			GPUTextureUsage.RENDER_ATTACHMENT,
		format: 'depth24plus-stencil8',
		dimension: '2d'
	});

	var layout = device.createPipelineLayout({
		bindGroupLayouts: []
	});

}