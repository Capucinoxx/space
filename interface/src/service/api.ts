const send = async (
  url: string,
  params: { [key: string]: any },
  success_cb: ((response: ArrayBuffer) => void) | undefined = undefined,
  error_cb: ((error: ArrayBuffer) => void) | undefined = undefined,
): Promise<void> => {
  const method = params.method || 'GET';
  const headers = params.headers || {};
  let body = format_body(params.body);

  if (method === 'GET' && body !== undefined) {
    url += '?' + body;
    body = undefined;
  }

  const options: RequestInit = {
    method: method,
    headers: headers,
    body: body,
  };

  const response = await fetch(url, options);

  if (response.ok && response.status < 300)
    if (success_cb !== undefined)
      success_cb(await response.arrayBuffer());
    else
      if (error_cb !== undefined)
        error_cb(await response.arrayBuffer());
};

const format_body = (body: any): string | undefined => {
  if (body === undefined)
    return undefined;

  if (typeof body === 'string')
    return body;

  if (body instanceof FormData)
    return [...body.entries()]
      .map((el) => `${encodeURIComponent(el[0])}=${encodeURIComponent(el[1].toString())}`)
      .join('&');

  if (typeof body === 'object')
    return JSON.stringify(body);

  return undefined;
}

export { send };